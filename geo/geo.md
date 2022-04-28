---
layout: geo
id: geo
---
 


  <iframe style=" border-width: 0px; width: 100%; min-height: 800px;" name="main" src="https://vavolkl.web.cern.ch/vavolkl/geo/index.htm?file=files/fcchh-geo.root&item=world&opt=clipxyz&nobrowser" target="main">

powered by js-root.
<!--

<script type="text/javascript" src="vavolkl.web.cern.ch/vavolkl/geo/index.htm?file=files/fcchh-geo.root&item=world&opt=clipxyz"></script>
 <div id="simpleGUI" path="./geo/" files="fcchh-geo.root">
   loading scripts ...
 </div>


   <script src="https://root.cern.ch/js/latest/scripts/JSRootCore.js" type="text/javascript"></script>



   <script type='text/javascript'>

     // absolute file path can be used as well

     //var geom_file = "https://root.cern.ch/js/files/geom/evegeoshape.json.gz",
     //var geom_file = "http://vavolkl.web.cern.ch/vavolkl/geo/fcchh-geo.root",
     var geom_file = "geo/fcchh-geo.root",

         tracks_file = "https://root.cern.ch/js/latest/files/geom/eve_tracks.root";

     

     JSROOT.NewHttpRequest(geom_file, 'object', function(obj) {

        JSROOT.draw("drawing", obj, "", function(geo_painter) {

           JSROOT.OpenFile(tracks_file, function(file) {

              file.ReadObject("tracks;1", function(tracks) {

                 // one could simply draw list of tracks

                 // JSROOT.draw("drawing", tracks, "same");

                 

                 // or use method of TGeoPainter to draw extras

                 geo_painter.drawExtras(tracks, "tracks");

                 

                 // use this method if one want to remove all extra objects 

                 // geo_painter.clearExtras(); 

              });

           });

        });

     }).send();

     

   </script>

<div id="drawing" style="width:800px; height:600px"></div>
-->
