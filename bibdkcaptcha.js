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
                    url:'create_login/playaudiocaptcha',
                    success: playAudio
                    });
                return false;
            });
            
            function updateCaptcha(){
                $.get('create_login/refreshcatptcha',
                    function(data){
                        $(data).prependTo("#captcha");
                    });
                return false;
            }
            
            function playAudio(){
                console.log('playing audio');
            }
        }
    }
})(jQuery);