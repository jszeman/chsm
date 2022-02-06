// import { Gui } from './../modules/gui.mjs';
// import { Model } from '../modules/model.js';
// import { state_machine } from '../data.js';
// import { App } from './../main.js';

class New_project{
    constructor(){
        // New module
        this.module.name = document.getElementById("module_name");
        this.module.version = document.getElementById("module_version");
        this.module.description = document.getElementById("module_description");
        this.module.location = document.getElementById("module_location");
        // Batch
        this.batch_cb = document.getElementById("batch_cb");
        this.batch_location_cb = document.getElementById("batch_location");
        // CMake
        this.cmake_cb = document.getElementById("cmake_cb");
        this.cmake_default_loc_cb = document.getElementById("cmake_default_cb");

        // Browse buttons
        this.new_module_loc_browse_btn = document.getElementById("btn-new-module-loc-browse");

        // Buttons
        this.cancel_btn = document.getElementById("btn-cancel");
        this.cancel_btn.addEventListener("onclick" , e => this.state_select("CANCEL")); 
        this.generate_btn = document.getElementById("btn-generate");
        this.generate_btn.addEventListener("onclick" , e => this.state_select("GENERATE")); 
    }

    browse_loc()
    {
        eel.browse_loc();
    }
    genarate_module_architect(){}
    generate_batch(){}
    generate_cmake(){}

    generate_project()
    {

    }

    cancel_project_window()
    {
        console.log("Cancel button clicked");
        window.close();
    }

    state_select(event)
    {
        switch(event)
        {
            case "CANCEL": 
                cancel_project_window();
                break;
            case "GENERATE":
                generate_project();
                break;  
            case "BROWSE":
                eel.browse_loc();
                break;
        }

    }
}


let new_project = new New_project();