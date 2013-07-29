(function($) {
    Drupal.behaviors.bibdkcaptcha = {
        attach : function(context) {
            var basepath = Drupal.settings.basePath;

            $("#bibdkcaptcha-controls-refreshbtn").click(function() {
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
                var url = basepath + mainUrl;

                if($.browser['msie']){
                    type = 'mp3';
                }

                var thissound = document.createElement('audio');
                thissound.preload = 'auto';
                thissound.autoplay = 'auto';
                thissound.src = (url + type);
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
