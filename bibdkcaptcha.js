    (function($) {
    Drupal.behaviors.bibdkcaptcha = {
        attach : function(context) {
            var host = "http://" + document.location.hostname;
            
            var basePath = Drupal.settings.basePath;
            console.log(basePath);
            if(basePath.length){
                host = host + basePath;
                console.log(host);
            }
            console.log(basePath);

            
            $("#bibdkcaptcha-controls-refreshbtn").click(function() {
                jQuery.ajax({
                    type: 'GET',
                    url:'captcha/refreshcatptcha',
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
                console.log($.browser);
                if($.browser['safari'] || $.browser['mozilla']){
                    type = 'wav';
                    $("#bibdkcaptcha-controls-playcaptcha").attr("type","audio/x-wav");
                }
                var mainUrl = 'captcha/playaudiocaptcha'+"/"+$('input[name=captcha_sid]').val()+"/"+$('input[name=captcha_token]').val()+"/"+type;
                url = host + mainUrl;
                console.log(url);
                if($.browser['msie']){ // && $.browser['version'] <= 8){
                    if($("#bibdkcaptcha-controls-playcaptcha")){
                        $("#bibdkcaptcha-controls-playcaptcha").remove();
                    }
                    
                    $("#bibdkcaptcha-controls").append('﻿<embed id="embedobj" hidden="true" src="'+url+'" autoplay="true" autostart="true"></embed>');
                } else {
                    $("#bibdkcaptcha-controls-playcaptcha").attr("src", url);
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