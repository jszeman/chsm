# Internal Representation (IR) reference

When you draw a state machine in the Cgen GUI, it gets saved in an internal format that’s essentially just a description of the graphics. This is what goes into the `.html` files. While it *does* contain all the info we need for code generation, trying to use it directly for that would be... well, a bit crazy.

When Cgen starts generating code, the first step is to preprocess the graphical data into a format that's more suitable for code generation. This involves tasks like:  

- Parsing state and transition texts into signals, guards, function calls, and transition targets.
- Discovering transition paths: the list of functions to call when moving from one state to another.
- Discovering init paths for initializing states.
- Flattening the state machine: retaining only simple states while adding ancestor event handlers to them. This simplifies the code generation process for even the most complex, deeply nested state hierarchies.

The outcome of this preprocessing step is the **IR** (Internal Representation), which is then passed to the Jinja template to produce the final output.

In this document, we'll dive into the internals of the **IR** (Internal Representation). By understanding the details, you'll be better equipped to extend the template from the previous chapter and take advantage of more advanced features in your state machine code generation.

Feel free to skip this chapter—the IR is quite self-explanatory. You can always revisit it if something needs clarification.

I recommend opening the **IR** of the [test state machine](../../crf/test/doc/chsm_test_machine4.json) and referencing it as you read this document.

## IR overview

The IR (Intermediate Representation) organizes the key data for the state machine. Here's what each attribute represents:

- **`states`**: Contains the **`__top__`** state and all the simple states.
- **`user_funcs`**: A list of functions used in the drawing without extra parameters.
- **`user_guards`**: A list of guards (conditions) without additional parameters.
- **`user_signals`**: A list of signals present in the drawing.
- **`user_inc_funcs`**: Functions called with extra parameters in the drawing.
- **`notes`**: Notes linked to functions and signals.
- **`template_params`**: Parameters provided in the job file for customization.
- **`raw_data`** *(optional)*: Raw graphical data, useful for exporting graphics to other formats, such as the `drawio` template.

## data.states

The value of the `states` attribute is a dictionary with the **`__top__`** state and all simple states.

### data.states.\_\_top\_\_

The **`__top__`** state contains the initial transition information needed when the state machine processes the `init` signal. Here are the interesting parts of the data structure:

- **`__top__.sys_signals.init.funcs`**: A list of functions to execute. Each item is a pair consisting of a function name and its parameters.
- **`__top__.sys_signals.init.target_title`**: The name of the initial state.

You can safely ignore all other data here.

### data.states.state\_N

All simple state attributes are named `state_N`, where `N` is the state identifier. Each state attribute is a dictionary with the following attributes:

- **`signals`**: Describes the event handlers for signals processed by the state.
- **`guards`**: Describes completion guards—evaluated after events that don’t trigger a state change.
- **`title`**: The name of the state.
- **`num`**: The state’s unique identifier.

All other attributes in the state dictionary should be ignored.

#### data.states.state\_N.signals

A dictionary for the event handlers of the signals processed by the state, with the following attributes:

- **`name`**: The name of the signal.
- **`guards`**: A dictionary containing all the guards to evaluate when processing the event.

##### data.states.state\_N.signals.guards

This is a dictionary containing all the guards to evaluate when processing an event. Each attribute is either in the form `guard_func(guard_param)` or an empty string (`""`). The `""` guard indicates that no `if` statement needs to be generated for the event handler. Each value in the dictionary is a dictionary with the following attributes:

- **`guard_func`**: The name of the guard function.
- **`guard_param`**: Optional parameters to pass to the guard function.
- **`funcs`**: A list of functions to call if the guard function evaluates to `true`. Each entry is a pair of a function name and its parameters.
- **`target_title`**: The title of the target state to activate after the functions are executed. An empty string means no transition is required.
- **`target_type`**: Specifies the type of the target:
  - **`normal`**: The target is a standard state.
  - **`call`**: The target is a function call.
- **`target_params`**: Parameters to pass to a call target. This attribute is only present if **`target_type`** is **`call`**.

All other attributes in this dictionary should be ignored.

#### data.states.state\_N.guards

The **`guards`** attribute in a state is a dictionary for completion guards. These are evaluated after handling any event that did not result in a transition to another state.

The internals of the completion guards are identical to the signal guards described in the section [data.states.state_N.signals.guards](#datastatesstate_nsignalsguards).

## data.user_funcs

This is a list of function names used in the drawing that do not have any extra parameters. Since they lack custom arguments, a C header template can generate declarations for these functions.

## data.user_guards

This is a list of function names used in the drawing as guard functions without any extra parameters. Since they don’t have custom arguments, a C header template can generate declarations for these functions.

## data.user_inc_funcs

This is a list of functions and guards used in the drawing that are called with non-empty parameter lists. If the target language requires declarations, it is the user’s responsibility to provide them.

## data.user_signals

This is a list of all signals that appear at least once in the drawing.

## data.notes

In Cgen, you can attach notes to almost any string by clicking on the text in the drawing and entering content into the `Text` field. These notes are stored in this dictionary. The default Cgen C templates use these notes to generate comments for the associated functions and signals.

## data.template_params

These are parameters from the output job descriptor, which can be used to create more flexible templates.