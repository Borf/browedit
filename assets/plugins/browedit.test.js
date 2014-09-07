(function (){
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
            
            var cell = browedit.map.gnd.getCell(10, 10);
            cell.setHeight(0, cell.getHeight(0) + 10);
        }
    };

})();

