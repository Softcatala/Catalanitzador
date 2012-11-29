<?php


?>
    <script>
    $(function() {
        $( "#from" ).datepicker({
            defaultDate: "-1d",
            changeMonth: true,
            dateFormat: 'dd/mm/yy',
            onClose: function( selectedDate ) {
                $( "#to" ).datepicker( "option", "minDate", selectedDate );
            }
            
        });
        $( "#to" ).datepicker({
            defaultDate: "today",
            changeMonth: true,
            dateFormat: 'dd/mm/yy',
            onClose: function( selectedDate ) {
                $( "#from" ).datepicker( "option", "maxDate", selectedDate );
            }
        });
        
        $('h3.log-id').click(function(){
            id = $(this).attr('id');
            $('#content-'+id).toggle();
        });
        
        $('#hide-empty').change(function(){
            if($(this).is(':checked')) {
                $('.wrap-log-empty').hide();
            } else {
                $('.wrap-log-empty').show();
            }
        });
        
        $('#hide-empty').prop('checked',true).trigger("change");
    });
    </script><br /><br />
    <h2>Logs</h2>
    <form action="/stats/?show=logs" method="post">
        <label for="from">Data inicial:</label>
        <input type="text" id="from" name="from" value="<?=$_POST['from']?>"/><br />
        <label for="to">Data final:</label>
        <input type="text" id="to" name="to" value="<?=$_POST['to']?>"/><br />
        <input type="submit" /><br /><br />
        <label for="hide-empty">Amaga els logs buits</label>
        <input type="checkbox" name="hide-empty" id="hide-empty" />
    </form>
    
<?php
    if(isset($_POST['from']) && !empty($_POST['from']) &&
            isset($_POST['to']) && !empty($_POST['to'])) {
        
        global $db;
        require 'class.logs.php';
        
        $Logs = new Logs($db);
        $Logs->print_logs($_POST['from'],$_POST['to']);
    }
?>