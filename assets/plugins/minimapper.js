var maps = [
   "alb2trea",
    "alberta",
    "alberta_in",
    "alde_dun01",
    "alde_dun02",
    "alde_dun03",
    "alde_dun04",
    "aldeba_in",
    "aldebaran",
    "anthell01",
    "anthell02",
    "arena_room",
    "c_tower1",
    "c_tower2",
    "c_tower3",
    "c_tower4",
   "gef_dun00",
   "gef_dun01",
   "gef_dun02",
   "gef_dun03",
   "gef_fild00",
   "gef_fild01",
   "gef_fild02",
   "gef_fild03",
   "gef_fild04",
   "gef_fild05",
   "gef_fild06",
   "gef_fild07",
   "gef_fild08",
   "gef_fild09",
   "gef_fild10",
   "gef_fild11",
   "gef_fild12",
   "gef_fild13",
   "gef_fild14",
   "gef_tower"
];


function bla(browedit, mapId) {
    browedit.map.load("data/" + maps[mapId]);
    browedit.display.showGrid(false);
    browedit.display.showLightmap(true);
    browedit.display.showLights(false);
    browedit.display.showEffects(false);
    browedit.display.showSounds(false);

    browedit.display.camera.setOrtho(true);
    browedit.display.camera.setCenter(browedit.map.gnd.getWidth() * 5, browedit.map.gnd.getHeight() * 5);
    browedit.display.camera.setAngle(90);
    browedit.display.camera.setDirection(0);
    browedit.display.camera.setDistance(browedit.map.gnd.getHeight() * 5);

    browedit.afterDraw(function () {
        if(mapId < maps.length-1)
            bla(browedit, mapId+1)
    }, this);

}

(function () {
    return {
        "name" : "Minimapper plugin",
        "author": "Borf",
        "version": 1,

        "menu": "plugins/Create Minimaps",

        "init" : function (browedit) {
        },
        "action": function (browedit) {
            bla(browedit, 0);
        }


    };

})();

