
export class New_module{
    constructor(){
        // New module
        this.name = document.getElementById("module_name");
        this.version = document.getElementById("module_version");
        this.description = document.getElementById("module_description");
        this.module_location = document.getElementById("module_location");
        this.browse_btn = document.getElementById("btn-new-module-loc-browse");
        this.browse_btn.addEventListener("click" , e => this.state_select("MODULE_BROWSE"));
        // Batch
        this.batch_cb = document.getElementById("batch_cb");
        this.batch_default_loc_cb = document.getElementById("batch_default_loc_cb");
        this.batch_location = document.getElementById("batch_location");
        this.batch_browse_btn = document.getElementById("btn-batch-loc-browse");
        this.batch_browse_btn.addEventListener("click" , e => this.state_select("BATCH_BROWSE"));
        // CMake
        this.cmake_cb = document.getElementById("cmake_cb");
        this.cmake_default_loc_cb = document.getElementById("cmake_default_cb");
        this.cmake_location = document.getElementById("cmake_location");
        this.cmake_target_selector = document.getElementById("target-type-selector");
        this.cmake_browse_btn = document.getElementById("btn-cmake-loc-browse");
        this.cmake_browse_btn.addEventListener("click" , e => this.state_select("CMAKE_BROWSE"));

        // Buttons
        this.cancel_btn = document.getElementById("btn-cancel");
        this.cancel_btn.addEventListener("click" , e => this.state_select("CANCEL")); 
        this.generate_btn = document.getElementById("btn-generate");
        this.generate_btn.addEventListener("click" , e => this.state_select("GENERATE")); 

        this.browse_source = 0;

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
                eel.generate_module(this.name.value, 
                                    this.version.value, 
                                    this.description.value, 
                                    this.module_location.value, 
                                    this.batch_location.value, 
                                    this.cmake_target_selector.value,
                                    this.batch_cb.checked,
                                    this.cmake_cb.checked);
                console.log(this);
                break;  
            case "MODULE_BROWSE":
                this.browse_source = "MODULE_BROWSE"
                eel.browse_dir_path();
                break;
            case "BATCH_BROWSE":
                this.browse_source = "BATCH_BROWSE"
                eel.browse_dir_path();
                break;
            case "CMAKE_BROWSE":
                this.browse_source = "CMAKE_BROWSE"
                eel.browse_dir_path();
                break;
        }

    }
}

window.nm = new New_module();

eel.expose(set_choosen_path_to_input_text);
function set_choosen_path_to_input_text(path)
{
    switch(nm.browse_source)
    {
        case "MODULE_BROWSE":
            nm.module_location.value = path;
            if(nm.batch_default_loc_cb.checked)
                nm.batch_location.value = path;
            if(nm.cmake_default_loc_cb.checked)
                nm.cmake_location.value = path;
            break;
        case "BATCH_BROWSE":
            nm.batch_location.value = path;
            break;
        case "CMAKE_BROWSE":
            nm.cmake_location.value = path;
            break;

    }
}

eel.expose(successful_generate)
function successful_generate()
{
    nm.generate_btn.style.backgroundColor = "green";

    console.log("Successfully generated!");
}