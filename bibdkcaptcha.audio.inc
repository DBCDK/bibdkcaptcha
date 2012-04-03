<?php

/**
 * Generate a MP3 file by concatenating individual files
 * @param array $letters  Array of letters to build a file from
 * @return string  MP3 file data
 */
function bibdkcaptcha_generate_audio_mpeg($letters, $case_sentitive) {
  $out_data = '';
  $lang = 'eng';

  global $language;
  if ($language->language == 'da') {
    $lang = 'dan';
  }
  $audio_path = drupal_get_path('module', 'bibdkcaptcha') . '/audio/' . $lang . '/';

  $letters = str_split($letters);
  if (!$case_sentitive && $lang == 'dan') {
    $letters = bibdkcaptcha_generate_case_sensitive($letters); //Upper-/lowercase is currently supported in dansih only
  }
  foreach ($letters as $letter) {
    $filename = $audio_path . strtoupper($letter) . '.wav';
    $fp = fopen($filename, 'r');
    $data = fread($fp, filesize($filename)); // read file in
    $data = bibdkcaptcha_scramble_audio_data($data);
    $out_data .= $data;
    fclose($fp);
  }
  return $out_data;
}

/**
 * Generate a wav file by concatenating individual files
 * @param array $letters  Array of letters to build a file from
 * @return string  WAV file data
 */
function bibdkcaptcha_generate_audio_wav($letters, $case_sentitive) {
  $out_data = '';

  $out_data = '';
  $lang = 'eng';

  global $language;
  if ($language->language == 'da') {
    $lang = 'dan';
  }
  $audio_path = drupal_get_path('module', 'bibdkcaptcha') . '/audio/' . $lang . '/';

  $letters = str_split($letters);
  if (!$case_sentitive && $lang == 'dan') {
    $letters = bibdkcaptcha_generate_case_sensitive($letters); //Upper-/lowercase is currently supported in dansih only
  }

  $data_len = 0;
  foreach ($letters as $letter) {
    $filename = $audio_path . strtoupper($letter) . '.wav';

    $fp = fopen($filename, 'rb');

    $file = array();

    $data = fread($fp, filesize($filename)); // read file in

    $header = substr($data, 0, 36);
    $body = substr($data, 44);

    $data = unpack('NChunkID/VChunkSize/NFormat/NSubChunk1ID/VSubChunk1Size/vAudioFormat/vNumChannels/VSampleRate/VByteRate/vBlockAlign/vBitsPerSample', $header);

    $file['sub_chunk1_id'] = $data['SubChunk1ID'];
    $file['bits_per_sample'] = $data['BitsPerSample'];
    $file['channels'] = $data['NumChannels'];
    $file['format'] = $data['AudioFormat'];
    $file['sample_rate'] = $data['SampleRate'];
    $file['size'] = $data['ChunkSize'] + 8;
    $file['data'] = $body;

    if (($p = strpos($file['data'], 'LIST')) !== false) {
      // If the LIST data is not at the end of the file, this will probably break your sound file
      $info = substr($file['data'], $p + 4, 8);
      $data = unpack('Vlength/Vjunk', $info);
      $file['data'] = substr($file['data'], 0, $p);
      $file['size'] = $file['size'] - (strlen($file['data']) - $p);
    }

    $files[] = $file;
    $data = null;
    $header = null;
    $body = null;

    $data_len += strlen($file['data']);

    fclose($fp);
  }

  $out_data = '';
  for ($i = 0; $i < sizeof($files); ++$i) {
    if ($i == 0) { // output header
      $out_data .= pack('C4VC8', ord('R'), ord('I'), ord('F'), ord('F'), $data_len + 36, ord('W'), ord('A'), ord('V'), ord('E'), ord('f'), ord('m'), ord('t'), ord(' '));

      $out_data .= pack('VvvVVvv', 16, $files[$i]['format'], $files[$i]['channels'], $files[$i]['sample_rate'], $files[$i]['sample_rate'] * (($files[$i]['bits_per_sample'] * $files[$i]['channels']) / 8), ($files[$i]['bits_per_sample'] * $files[$i]['channels']) / 8, $files[$i]['bits_per_sample']);

      $out_data .= pack('C4', ord('d'), ord('a'), ord('t'), ord('a'));

      $out_data .= pack('V', $data_len);
    }

    $out_data .= $files[$i]['data'];
  }

  $out_data = bibdkcaptcha_scramble_audio_data($out_data);
  return $out_data;
}

/**
 * Randomly modify the audio data to scramble sound and prevent binary recognition.
 * Take care not to "break" the audio file by leaving the header data intact.
 * @param $data Sound data in mp3 of wav format
 */
function bibdkcaptcha_scramble_audio_data($data) {
  $start = strpos($data, 'data') + 4; // look for "data" indicator
  if ($start === false)
    $start = 44;  // if not found assume 44 byte header

  $start += rand(1, 4); // randomize starting offset
  $datalen = strlen($data) - $start;
  $step = 1;

  for ($i = $start; $i < $datalen; $i += $step) {
    $ch = ord($data{$i});
    if ($ch == 0 || $ch == 255)
      continue;

    if ($ch < 16 || $ch > 239) {
      $ch += rand(-6, 6);
    }
    else {
      $ch += rand(-12, 12);
    }

    if ($ch < 0)
      $ch = 0; else if ($ch > 255)
      $ch = 255;

    $data{$i} = chr($ch);

    $step = rand(1, 4);
  }

  return $data;
}

/**
 * Helper function to support case sensitivness in danish speaken audio captcha. 
 * @param array $letters
 * @return array $sens_letters with strings to support upper- and lowercase. Dansih only
 */
function bibdkcaptcha_generate_case_sensitive($letters) {
  $sens_letters = array();
  foreach ($letters as $letter) {
    if (!is_numeric($letter)) {
      if (ctype_upper($letter)) {
        $sens_letters[] = 'stort';
      }
      else {
        $sens_letters[] = 'lille';
      }
    }
    $sens_letters[] = $letter;
  }
  return $sens_letters;
}