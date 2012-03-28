<?php

function bibdkcaptcha_generate_audio($letters) {
  $out_data = '';
  $lang = 'eng';

  global $language;
  if ($language->language == 'da') {
    $lang = 'dan';
  }
  $audio_path = drupal_get_path('module', 'bibdkcaptcha') . '/audio/' . $lang . '/';

  $letters = str_split($letters);

  foreach ($letters as $letter) {
    $filename = $audio_path . strtoupper($letter) . '.mp3';
    $fp = fopen($filename, 'r');
    $data = fread($fp, filesize($filename)); // read file in
    #$data = bibdkcaptcha_scramble_audio_data($data, 'mp3'); //TODO should included when audio is working
    $out_data .= $data;

    fclose($fp);
  }

  return $out_data;
}

/**
 * Randomly modify the audio data to scramble sound and prevent binary recognition.<br />
 * Take care not to "break" the audio file by leaving the header data intact.
 *
 * @since 2.0
 * @access private
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