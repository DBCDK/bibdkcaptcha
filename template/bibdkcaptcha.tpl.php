<?php
/**
 * @file
 * Default theme implementation for displaying captcha refresh and audio button.
 */
?>


<div id="bibdkcaptcha-controls">
  <?php print $captcha; ?>
  <br />
  <img id="bibdkcaptcha-controls-refreshbtn" src="<?php print $module_path; ?>/graphics/refresh.gif" />
  <img id="bibdkcaptcha-controls-audiobtn" src="<?php print $module_path; ?>/graphics/audio_icon.gif" />
  <audio id="bibdkcaptcha-controls-playcaptcha-embed" autoplay="true" autostart="true" hidden="true" src="" ></audio>
</div>
