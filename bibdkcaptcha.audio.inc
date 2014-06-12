<?php

/**
 * Generate a MP3 file by concatenating individual files
 *
 * @param String $letters  String of letters to build a file from
 * @param Boolean $case_sentitive
 * @return String  MP3 file data
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
    $filename = $audio_path . strtoupper($letter) . '.mp3';
    $fp = fopen($filename, 'r');
    $data = fread($fp, filesize($filename)); // read file in
    $data = bibdkcaptcha_scramble_audio_data_mpeg($data);
    $out_data .= $data;
    fclose($fp);
  }
  return $out_data;
}

function bibdkcaptcha_scramble_audio_data_mpeg($data) {
  $start = 4; // 4 byte (32 bit) frame header

  $start += rand(1, 64); // randomize starting offset
  $datalen = strlen($data) - $start - 256; // leave last 256 bytes unchanged

  for ($i = $start; $i < $datalen; $i += 64) {
    $ch = ord($data{$i});
    if ($ch < 9 || $ch > 119) {
      continue;
    }

    $data{$i} = chr($ch + rand(-8, 8));
  }
  return $data;
}

/**
 * Generate a wav file by concatenating individual files
 *
 * @param array $letters  String of letters to build a file from
 * @param $case_sentitive Boolean
 * @return string  WAV file data
 */
function bibdkcaptcha_generate_audio_wav($letters, $case_sentitive) {
  $silence = variable_get('bibdk_captcha_silence', 1500);
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
    fclose($fp);
    $wave = bibdkcaptcha_wave_riff($data);
    $wave['format'] = $wave['fmt']['AudioFormat'];
    $wave['channels'] = $wave['fmt']['NumChannels'];
    $wave['sample_rate'] = $wave['fmt']['SampleRate'];
    $wave['bits_per_sample'] = $wave['fmt']['BitsPerSample'];
    $padding = $wave['channels'] * $wave['sample_rate'] * $wave['bits_per_sample'] / 8 * $silence / 1000;
    $padding -= $padding % $wave['fmt']['BlockAlign'];
    $data_len += strlen($wave['data']['audio']) + $padding;
    $wave['data'] = $wave['data']['audio'] . str_pad('', $padding, "\x00");
    $files[] = $wave;
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
 *
 * @param $data String data in mp3 of wav format
 * @return String
 */
function bibdkcaptcha_scramble_audio_data($data) {
  $start = strpos($data, 'data') + 4; // look for "data" indicator
  if ($start === FALSE) {
    $start = 44;
  } // if not found assume 44 byte header

  $start += rand(1, 4); // randomize starting offset
  $datalen = strlen($data) - $start;
  $step = 1;

  for ($i = $start; $i < $datalen; $i += $step) {
    $ch = ord($data{$i});
    if ($ch == 0 || $ch == 255) {
      continue;
    }

    if ($ch < 16 || $ch > 239) {
      $ch += rand(-6, 6);
    }
    else {
      $ch += rand(-12, 12);
    }

    if ($ch < 0) {
      $ch = 0;
    }
    elseif ($ch > 255) {
      $ch = 255;
    }

    $data{$i} = chr($ch);

    $step = rand(1, 4);
  }

  return $data;
}

/**
 * Helper function to support case sensitivness in danish speaken audio captcha.
 *
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

/**
 * Reads the data subchunk from the beginning of a data string. Verification that
 * this is a data subchunk can be done by checking that SubChunkID is 0x64617461
 * (big-endian) spelling 'data'.
 *
 * @param array $data
 * @return array
 */
function bibdkcaptcha_wave_data($data) {
  $audio = unpack('NSubChunkID/VSubChunkSize', $data);
  $audio['size'] = $audio['SubChunkSize'] + 8;
  $audio['audio'] = substr($data, 8, $audio['SubChunkSize']);
  return $audio;
}

/**
 * Reads the fmt subchunk from the beginning of a data string. Verification that
 * this is a fmt subchunk can be done by checking that SubChunkID is 0x666d7420
 * (big-endian) spelling 'fmt ' (note the space!).
 *
 * @param array $data
 * @return array
 */
function bibdkcaptcha_wave_fmt($data) {
  $fmt = unpack('NSubChunkID/VSubChunkSize/vAudioFormat/vNumChannels/VSampleRate/VByteRate/vBlockAlign/vBitsPerSample', $data);
  $fmt['size'] = $fmt['SubChunkSize'] + 8;
  return $fmt;
}

/**
 * Reads the riff chunk from the beginning of a data string. Verification that
 * this is a riff chunk can be done by checking that ChunkID is 0x52494646
 * (big-endian) spelling 'RIFF'.
 *
 * @param array $data
 * @return array
 */
function bibdkcaptcha_wave_riff($data) {
  $riff = unpack('NChunckID/VChunkSize/NFormat', $data);
  $riff['size'] = $riff['ChunkSize'] + 8;
  $data = substr($data, 12);
  $riff['fmt'] = bibdkcaptcha_wave_fmt($data);
  $data = substr($data, $riff['fmt']['size']);
  $riff['data'] = bibdkcaptcha_wave_data($data);
  return $riff;
}
