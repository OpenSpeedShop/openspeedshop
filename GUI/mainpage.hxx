/*!\mainpage GUI Interface Overview
 *
 *\section intro_the_goal The Goal
 *The goal of the GUI is to provide an intuitive, easy to use, portal to an
 *otherwise complex and overwhelming application.   In the case of Open
 *Speed Shop, we are attempting
 *to put an intuitive interface between the user and their applications
 *performance experiment.   It's a layer that will help the user ask the
 *correct questions and get the correct meaningful answer back regarding,
 *"Where is my application spending all its time and  how can I make it go
 *faster."
 *
 *One objective to this goal is to provide a GUI framework that will provide
 *common consistent base functionality.   One that will prevent a complex
 *set of tools from getting window-itis, yet providing the user with all
 *the necessary control and clues/hints to achieve their end goal.    The
 *GUI base will also provide the necessary flexibility to add new functionality
 *as other developers begin to develop add-ons (plugins) to improve the core
 *set of tools provided.
 *
 *\section intro_to_gui_section The GUI Infrastructure
 *
 *The GUI is broken into 3 basic parts.   The Main Window, PanelContainers,
 *and Panels. 
 *
 *\subsection intro_to_mainwindow The Main Window
 *
 *There is only one Main Window and it is the owner of the 
 *master PanelContainer.   There is only one master PanelContainer and when 
 *it is closed, the GUI exits.
 *
 *The Main Window will handle high level commands like managing the main
 *menu toolbar.  This menu bar is dynamically
 *created from the list of dynamic GUI plugins.   
 *  
 *\subsection intro_to_panel_containers PanelContainers
 *
 *The PanelContainers will provide all the underlying
 *functionality for screen layout, local menu management, dive functionality,
 *drag-n-drop, and communication broker.
 *
 *A PanelContainer is an area (container) on the screen where Panels can
 *be placed and managed. 
 *
 *Each PanelContainer will have a simple communication broker to allow
 *Panels to communicate between local and remote Panels. 
 *
 *A PanelContainer is responsible for interacting with the user, via the
 *PanelContainer menu, to add, delete, resize, or move Panels.
 *
 *A PanelContainer can be split either horizontally or vertically into 2
 *PanelContainers. A PanelContainer can contain, one, many, or no Panels.
 *A PanelContainer has no specific knowledge of what a Panel is doing.
 *(i.e. I doesn't know anything about experiments. It just manages the
 *Panels space and any communication with other Panels.) 
 *
 *
 *\subsection intro_to_panels Panels
 *
 *A Panel is where all the nuts-n-bolts knowledge and logic exists for
 *an individual task.    
 *
 *They will handle Panel specific user interaction and interaction with the
 *Framework.
 *
 *A Panel is placed in a PanelContainer. Each Panel is managed by its
 *PanelContainer. A Panel is the base handle for all views. That is each
 *user defined Panel (i.e. performance experiment type) will have a Panel
 *type derived from a base class Panel. Examples of Panel types and their
 *functions would be:
 *
 *\par  o Wizard Panel:
 *The Wizard Panel would be responsible for interviewing the user to aid them
 *in selecting the right performance tool for the job.   After the interview
 *process was completed a command would be passed to the Framework for
 *action.  Upon completion the user would be notified and an appropriate
 *(default) report (Experiment Panel) would be generated for the user.  
  \par  o User Time Panel:
  \par  o  MPI Experiment Panel:
  \par  o  Call Graph Panel:
  \par  o  Source Panel
  \par  o  ...
 *
 *Panels will look like a "tabbed" view. A Panel can be moved around to
 *be placed near other Panels for association (communication) to be granted.
 *An example of this could be a User Time Panel could be placed near and
 *existing Source Panel, when the user clicks on an object in the User Time
 *Panel that relates to a source line, the nearest Source Panel would be
 *updated (repositioned) to that source line.
 *
 *\section intro_to_panel_plugins Why Panel Plugins?
 *
 *Panels will be loaded as plugins.  It will be the MainWindow's
 *responsibility to locate and configure the GUI to interact with whatever
 *plugins are currently located on the system.   The MainWindow's menus will
 *need to be dynamically configured based on the available plugins.   A
 *plugin directory (directories) will exist with 
 *dynamic libraries to be loaded.   The dynamic libraries may be
 *visible upon loading, or later (on demand), depending on their type.
 *
 *Using plugin technology allows for ease of exending this tool in the
 *future.   It also will keep this tool very lightweight as only the
 *functionality requested needs to be loaded.
 *
 *\section intro_to_gui_plugins All The To-Do About Dynamic GUI Plugins.
 *
 *A dynamic plugin is a GUI component.   A visible or hidden Panel.
 *Each plugin includes the following information to a common structure 
 *in the panel description.
 *
 *At initialization the default location (yet to be determined) or an
 *environment variable FUTURE_TOOL_PLUGIN_DIR is check for the available
 *plugins. Each plugin is openned and a menu structure is created from
 *the plugin header information.
 *
 *Here's what the plugin header information contains:
 *
 *local_plugin_info.h
 *
 *\li plugin_entry->grouping = "PLUGIN_GROUPING";
 *\par Grouping:
 *A general grouping name such as, Performance, Debugging, Static Analysis, ...
 *Currently only "Performance" is being used.
 *\li plugin_entry->show_immediate = SHOW_IMMEDIATE;
 *\par Show Immediate:
 *After loading the plugin file, load the panel to a panel container.
 *\li plugin_entry->plugin_description = "PLUGIN_DESCRIPTION";
 *\par Plugin Description:
 *A short description of the plugin panel (We'll show this with 'about' help.)
 *\li plugin_entry->plugin_name = "libTemplatePanel.so";
 *\par Plugin Name:
 *This is the dso name (Yes, it's redundant.)
 *\li plugin_entry->plugin_location = "$FUTURE_TOOL_PLUGIN_DIR";
 *\par Plugin Location:
 *This is a dynamic absolute path to the dso.
 *\li plugin_entry->plugin_entry_point = "panel_init";
 *\par Plugin Entry Point:
 *The generic entry point that is called to do the menu add. This is
 *separate from _init() to allow the user to still do 'other' initialization
 *if they need to before calling panel_init().
 *\li plugin_entry->menu_heading = "MENU_HEADING";
 *\par Menu Heading:
 *This is the actual heading that will be put to the toplevel menu headings
 *(The panel_init() function will loop through existing headers to see if
 *it needs to add this header or use an existing one.)
 *\li plugin_entry->menu_label = "MENU_LABEL";
 *\par Menu Label:
 *This is the menu entry under the "Menu Heading", added to the end
 *of the list. 
 *\li plugin_entry->panel_type = "MENU_LABEL";
 *\par Panel Type:
 *This is a unique string describing the panel type.
 *
 *
 *\section intro_to_starting_gui Into To How The GUI Launches Itself.
 *
 *The GUI is bundled into a dynamic library that is loaded on demand.  It's the *Command Line Interface (CLI) that launches the GUI.   By default the CLI 
 *will launch the GUI upon invocation of the OpenSpeedShop tool.   However,
 *the CLI can be started without starting the GUI ($ oss -cli) and then the 
 *GUI can loaded and initialize when needed.
 *
 *Upon invoking OpenSpeedShop ($ oss) the command line is parsed, and if the
 *GUI is requested the GUI library is loaded and launched.   We then drop into
 *an event loop that parsed both command lines and GUI events.
 *
 *When the GUI is loaded, the GUI looks for GUI plugins in the default
 *directory and in the FUTURE_TOOL_PLUGIN_DIR environment variable path.
 *Each file in the directory is opened and an internal entry point is 
 *queried.   If found the plugin manager calls the entry point, initializes
 *any exported menus, brings up the GUI, then drops into the main event
 *loop waiting for user interaction.
 *
 *\section intro_to_creating_a_panel How do I create the initial panel structure?
 *There's a script 'mknewpanel' in the TemplatePanel directory.
 *(.../Panels/TemplatePanel). Type in 'mknewpanel' to get a usage command.
 *
 *An example to create a new panel template for a panel called "TreePanel"
 *would be to type:
 *
 *$ cd Panels/TemplatePanel
 *
 *$ mknewpanel ../TreePanel TreePanel Performance "Tree Panel" 0 Performance
 *
 *This will create a new Panels directory (TreePanel), name the new panel
 *"TreePanel", and a menu item "Tree Panel" will be placed under the menu
 *entry "Performance". It won't be shown initially (i.e. '0') and it will
 *be associated with the "Performance" group.
 *
 *\section intro_to_default_panels What Panels To Provide by Default?
 *
 *\par o user time experiment
 *\par o input/output experiment
 *\par o hardware counter experiment
 *\par o PC sampling experiment
 *\par o MPI tracing experiment
 */
