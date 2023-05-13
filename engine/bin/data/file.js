var DerivedClass = Module.Application.extend("Application", {
    // __construct and __destruct are optional.  They are included
    // in this example for illustration purposes.
    // If you override __construct or __destruct, don't forget to
    // call the parent implementation!
    __construct: function() {
        this.__parent.__construct.call(this);
        this.setup();
    },
    setup() {
        console.log("setting up");
        this.model = new Module.Model;
        this.model.setup("heli.obj", false);
        this.angle = 0.0;
    },

    draw() {
        this.angle = this.angle + 0.01;
        this.up = new Module.vec3(0.0, 1.0, 0.0);
        this.scalar = new Module.vec3(0.1, 0.1, 0.1);
        this.model.model = new Module.mat4(1.0);
        this.model.model = Module.rotate(this.model.model, this.angle, this.up);
        this.model.model = Module.scale(this.model.model, this.scalar);
        this.model.draw();
    },
    __destruct: function() {
        this.__parent.__destruct.call(this);
    },
});

app = new DerivedClass;
Module.run(app);
