(function($) {
    Drupal.behaviors.bibdkcaptcha = {
        attach : function(context) {
            $("#bibdkcaptcha-controls-refreshbtn").click(function() {
                jQuery.ajax({
                    type: 'GET',
                    url:'create_login/refreshcatptcha',
                    success: updateCaptcha,
                    dataType: JSON
                });
            });            
            $("#bibdkcaptcha-controls-audiobtn").click(function() {
                playCaptcha();
                $("input#edit-captcha-response").focus();
            });
            function playCaptcha(){
                var type = 'mp3'; 
                if($.browser['safari']){
                    type = 'wav';
                    $('#bibdkcaptcha-controls-playcaptcha').attr("type","audio/x-wav");
                }
                var url = 'create_login/playaudiocaptcha'+"/"+$('input[name=captcha_sid]').val()+"/"+$('input[name=captcha_token]').val()+"/"+type;   
                if($.browser['msie']){
                    if($('#bibdkcaptcha-controls-playcaptcha')){
                        $('#bibdkcaptcha-controls-playcaptcha').remove();
                    }
                    $('#captcha').append('﻿<embed id="embedobj" hidden="true" src="' + url +'" ></embed>');
                } else {
                    $('#bibdkcaptcha-controls-playcaptcha').attr("src",url);
                }
            }
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