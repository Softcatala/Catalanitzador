<script type="text/javascript">

    if (typeof(OA_zones) != 'undefined') {
        var OA_zoneids = '';
        for (var zonename in OA_zones) OA_zoneids += escape(zonename+'=' + OA_zones[zonename] + "|");
        OA_zoneids += '&amp;nz=1';
    } else {
        var OA_zoneids = escape('1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|26|27|25|28|29|30|31|32');
    }

    if (typeof(OA_source) == 'undefined') { OA_source = ''; }
    var OA_p=location.protocol=='https:'?'https://ad.softcatala.org/www/delivery/spc.php':'http://ad.softcatala.org/www/delivery/spc.php';
    var OA_r=Math.floor(Math.random()*99999999);
    OA_output = new Array();

    var OA_spc="<"+"script type='text/javascript' ";
    OA_spc+="src='"+OA_p+"?zones="+OA_zoneids;
    OA_spc+="&amp;source="+escape(OA_source)+"&amp;r="+OA_r;
    OA_spc+=(document.charset ? '&amp;charset='+document.charset : (document.characterSet ? '&amp;charset='+document.characterSet : ''));

    if (window.location) OA_spc+="&amp;loc="+escape(window.location);
    if (document.referrer) OA_spc+="&amp;referer="+escape(document.referrer);
    OA_spc+="'><"+"/script>";
    document.write(OA_spc);

    function OA_show(name) {
        if (typeof(OA_output[name]) == 'undefined') {
            return;
        } else {
            document.write(OA_output[name]);
        }
    }

    function OA_showpop(name) {
        zones = window.OA_zones ? window.OA_zones : false;
        var zoneid = name;
        if (typeof(window.OA_zones) != 'undefined') {
            if (typeof(zones[name]) == 'undefined') {
                return;
            }
            zoneid = zones[name];
        }

        OA_p=location.protocol=='https:'?'https://ad.softcatala.org/www/delivery/apu.php':'http://ad.softcatala.org/www/delivery/apu.php';

        var OA_pop="<"+"script type='text/javascript' ";
        OA_pop+="src='"+OA_p+"?zoneid="+zoneid;
        OA_pop+="&amp;source="+escape(OA_source)+"&amp;r="+OA_r;
        if (window.location) OA_pop+="&amp;loc="+escape(window.location);
        if (document.referrer) OA_pop+="&amp;referer="+escape(document.referrer);
        OA_pop+="'><"+"/script>";

        document.write(OA_pop);
    }
var OA_fo = '';
OA_fo += "<"+"script type=\'text/javascript\' src=\'https://ad.softcatala.org/www/delivery/fl.js\'><"+"/script>\n";
document.write(OA_fo);

</script>

<div id="publisuperior">
<div id="publisuperior1">
<div class="publieti"><img title="Publicitat" alt="Publicitat" src="https://www.softcatala.org/img/publicitat.jpg"></div>
<div class="publicos">
<script type="text/javascript"><!--// <![CDATA[
    /* [id14] Softcatalà - Wiki Publisuperior esquerra */
    OA_show(14);
// ]]> --></script><noscript><a target="_blank" href="https://ad.softcatala.org/www/delivery/ck.php?n=d7f8765"><img border="0" alt="" src="http://ad.softcatala.org/www/delivery/avw.php?zoneid=14&amp;n=d7f8765" /></a></noscript>
</div>
</div>
<div id="publisuperior2">
<div class="publieti"><img title="Publicitat" alt="Publicitat" src="https://www.softcatala.org/img/publicitat.jpg"></div>
<div class="publicos">
<script type="text/javascript"><!--// <![CDATA[
/* [id17] Softcatalà - Genèric Publisuperior dreta */
OA_show(17);
// ]]> --></script><noscript><a target="_blank" href="https://ad.softcatala.org/www/delivery/ck.php?n=b548d5e"><img border="0" alt="" src="http://ad.softcatala.org/www/delivery/avw.php?zoneid=17&amp;n=b548d5e" /></a></noscript>
</div>
</div>
</div>
