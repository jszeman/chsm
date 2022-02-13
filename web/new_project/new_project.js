
export class New_project{
    constructor(){
        // New module
        this.name = document.getElementById("module_name");
        this.version = document.getElementById("module_version");
        this.description = document.getElementById("module_description");
        this.location = document.getElementById("module_location");
        this.browse_btn = document.getElementById("btn-new-module-loc-browse");
        this.browse_btn.addEventListener("click" , e => this.state_select("MODULE_BROWSE"));
        // Batch
        this.batch_cb = document.getElementById("batch_cb");
        this.batch_location_cb = document.getElementById("batch_location");
        this.batch_browse_btn = document.getElementById("btn-batch-loc-browse");
        this.batch_browse_btn.addEventListener("click" , e => this.state_select("BATCH_BROWSE"));
        // CMake
        this.cmake_cb = document.getElementById("cmake_cb");
        this.cmake_default_loc_cb = document.getElementById("cmake_default_cb");
        this.cmake_browse_btn = document.getElementById("btn-cmake-loc-browse");
        this.cmake_browse_btn.addEventListener("click" , e => this.state_select("CMAKE_BROWSE"));

        // Buttons
        this.cancel_btn = document.getElementById("btn-cancel");
        this.cancel_btn.addEventListener("click" , e => this.state_select("CANCEL")); 
        this.generate_btn = document.getElementById("btn-generate");
        this.generate_btn.addEventListener("click" , e => this.state_select("GENERATE")); 

        console.log("Constructor done!")
    }

    cancel_project_window()
    {
        console.log("Cancel button clicked");
        window.close();
    }

    state_select(event)
    {
        console.log("Event received!");
        console.log(event);
        switch(event)
        {
            case "CANCEL": 
                this.cancel_project_window();
                break;
            case "GENERATE":
                eel.generate_module(this.name, this.version, this.description, this.location);
                break;  
            case "MODULE_BROWSE":
                eel.browse_loc();
                break;
            case "BATCH_BROWSE":
                eel.browse_loc();
                break;
            case "CMAKE_BROWSE":
                eel.browse_loc();
                break;
        }

    }
}

window.np = new New_project();