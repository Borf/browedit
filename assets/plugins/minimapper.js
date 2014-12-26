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
    "c_tower4"
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

