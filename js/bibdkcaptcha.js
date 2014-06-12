(function($) {
    Drupal.behaviors.bibdkcaptcha = {
        attach : function(context) {
            var basepath = Drupal.settings.basePath;
            var audio_element = '#bibdkcaptcha-controls-playcaptcha-embed';

            $("#bibdkcaptcha-controls-refreshbtn").click(function() {
                $(audio_element)[0].src = '';
                jQuery.ajax({
                    type: 'GET',
                    url: basepath + 'captcha/refreshcaptcha',
                    success: updateCaptcha,
                    dataType: JSON
                });
            });

            $("#bibdkcaptcha-controls-audiobtn").click(function() {
                var type = 'wav';

                var mainUrl = 'captcha/playaudiocaptcha'+"/"+$('input[name=captcha_sid]').val()+"/"+$('input[name=captcha_token]').val()+"/"
                var url = basepath + Drupal.settings.pathPrefix + mainUrl;

                if($.browser['msie']){
                    type = 'mp3';
                }

                var thissound = document.createElement('audio');
                var thissound = $(audio_element)[0];
                thissound.preload = 'auto';
                thissound.autoplay = 'auto';
                if (thissound.src == window.location.href) {
                  thissound.src = (url + type);
                }
                thissound.play();

                $("input#edit-captcha-response").focus();
            });

            function updateCaptcha(data){
                var data = jQuery.parseJSON(data);
                $("#bibdkcaptcha-controls img:first-child").remove();
                $(data['captcha']).prependTo("#bibdkcaptcha-controls");

                $('input[name=captcha_sid]').val(data['sid']);
                $('input[name=captcha_token]').val(data['token']);

                return false;
            }
        }
    }
})(jQuery);
