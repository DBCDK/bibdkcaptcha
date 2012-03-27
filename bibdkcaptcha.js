(function($) {
    Drupal.behaviors.bibdkcaptcha = {
        attach : function(context) {
            $("#refreshbtn").click(function() {
                console.log('refreshbtn clicked');
                jQuery.ajax({
                    type: 'GET',
                    url:'create_login/refreshcatptcha',
                    success: updateCaptcha,
                    dataType: JSON
                    });
            });
            
            $("#audiobtn").click(function() {
                console.log('audiobtn clicked');
                $.get('user/create_login#refresh', 'rid=' + $(this).attr('id'), callback);

                // prevent entire page from reloading
                return false;
            });
            
            function updateCaptcha(){
                console.log('updateCaptcha');
                $.get('create_login/refreshcatptcha',
                    function(data){
                        console.log(data);
                    });
                return false;
            }
        }
    }
})(jQuery);