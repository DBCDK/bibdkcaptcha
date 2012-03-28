(function($) {
    Drupal.behaviors.bibdkcaptcha = {
        attach : function(context) {
            $("#refreshbtn").click(function() {
                $("#captcha img:first-child").fadeOut(200, function(){
                    $("#captcha img:first-child").remove();
                });
                jQuery.ajax({
                    type: 'GET',
                    url:'create_login/refreshcatptcha',
                    success: updateCaptcha,
                    dataType: JSON
                });
            });
            
            $("#audiobtn").click(function() {
                jQuery.ajax({
                    type: 'GET',
                    url:'create_login/playaudiocaptcha'+"/"+$('input[name=captcha_sid]').val()+"/"+$('input[name=captcha_token]').val(),
                    success: playAudio
                    //dataType: JSON
                });
            });
            
            function updateCaptcha(data){
                var data = jQuery.parseJSON(data);
                $(data['captcha']).prependTo("#captcha");
                $('input[name=captcha_sid]').val(data['sid']);
                $('input[name=captcha_token]').val(data['token']);
                return false;
            }
            
            function playAudio(data){
                console.log('hep');
            //var data = jQuery.parseJSON(data);
            //return false;
            }
        }
    }
})(jQuery);