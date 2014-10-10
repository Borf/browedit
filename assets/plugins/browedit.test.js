// implement JSON.stringify serialization
function stringify (obj) {

    var t = typeof (obj);
    if (t != "object" || obj === null) {

        // simple data type
        if (t == "string") obj = '"' + obj + '"';
        return String(obj);

    }
    else {

        // recurse array or object
        var n, v, json = [], arr = (obj && obj.constructor == Array);

        for (n in obj) {
            v = obj[n]; t = typeof (v);

            if (t == "string") v = '"' + v + '"';
            else if (t == "object" && v !== null) v = JSON.stringify(v);

            json.push((arr ? "" : '"' + n + '":') + String(v));
        }

        return (arr ? "[" : "{") + String(json) + (arr ? "]" : "}");
    }
};

(function () {
    return {
        "name" : "Test Plugin",
        "author": "Borf",
        "version": 1,

        "menu": "plugins/TestPlugin",

        "init" : function (browedit) {
            browedit.log("Hello from Javascript");
            browedit.log("We're running in browedit version " + browedit.version);
        },
        "action": function (browedit) {
            browedit.log("We can log properly");
            browedit.log("This map is " + browedit.map.gnd.getWidth() + " by " + browedit.map.gnd.getHeight());
            
            var obj = { 'hello': 'world', 'data' : [1,2,3,4] };
            browedit.log(stringify(obj));


            var map = {
                'file' : browedit.map.getFile(),
                'gnd': {
                    'version': 0,
                    'size' : [browedit.map.gnd.getWidth(), browedit.map.gnd.getHeight()]
                },
                'rsw': {
                    'version': browedit.map.rsw.getVersion(),
                    'water' :
                    {
                        'height' : browedit.map.rsw.water.getHeight(),
                        'type' : browedit.map.rsw.water.getType(),
                        'amplitude' : browedit.map.rsw.water.getAmplitude(),
                        'phase' : browedit.map.rsw.water.getPhase(),
                        'surfaceCurve' : browedit.map.rsw.water.getSurfaceCurve(),
                        'animSpeed' : browedit.map.rsw.water.getAnimSpeed()
                    }
                }
            };
            browedit.log(stringify(map));

//            cell.setHeight(0, cell.getHeight(0) + 10);
        }
    };

})();

