project('rg552-fancontrol-quick-setting-phosh-plugin', 'c')

name = 'rg552-fancontrol-quick-setting'
glib_ver = '2.76'
glib_ver_cmp = '>=@0@'.format(glib_ver)
prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))
plugins_dir = join_paths(libdir, 'phosh', 'plugins')
localedir = join_paths(prefix, get_option('localedir'))

gnome = import('gnome')
i18n  = import('i18n')

# Dependencies
gobject_dep = dependency('gobject-2.0', version: glib_ver_cmp)
gtk_dep = dependency('gtk+-3.0', version: '>=3.22')
libhandy_dep = dependency('libhandy-1', version: '>=1.1.90', fallback: ['libhandy', 'libhandy_dep'], default_options: ['introspection=disabled'])
libnm_dep = dependency('libnm', version: '>=1.14')
phosh_plugins_dep = dependency('phosh-plugins')

# Detect libphosh API version
# Due to the unstable API, the version changes frequently, 
# which makes it difficult to maintain in the package repository.
result = run_command('bash', 'scripts/detect-libphosh-api-version.sh')

if result.returncode() != 0
  error(result.stderr())
endif

libphosh_version = result.stdout().strip()
libphosh_dep = dependency('libphosh-' + libphosh_version)

plugin_deps = [
  gobject_dep,
  gtk_dep,
  libhandy_dep,
  libnm_dep,
  phosh_plugins_dep,
  libphosh_dep
]

# Resources
rg552_fancontrol_quick_setting_resources = gnome.compile_resources(
  'phosh-plugin-rg552-fancontrol-quick-setting-resources',
  'phosh-plugin-rg552-fancontrol-quick-setting.gresources.xml',
  c_name: 'phosh_plugin_rg552_fancontrol_quick_setting',
)

# Sources
rg552_fancontrol_quick_setting_plugin_sources = files(
  'phosh-plugin-rg552-fancontrol-quick-setting.c',
  'rg552-fancontrol-quick-setting.c',
)

# Plugin configuration
pluginconf = configuration_data()
pluginconf.set('name', name)
pluginconf.set('plugins_dir', plugins_dir)

# Plugin module
phosh_rg552_fancontrol_quick_setting_plugin = shared_module(
  'phosh-plugin-rg552-fancontrol-quick-setting',
  rg552_fancontrol_quick_setting_plugin_sources,
  rg552_fancontrol_quick_setting_resources,
  c_args: [
    '-DG_LOG_DOMAIN="phosh-plugin-@0@"'.format(name),
    '-DPLUGIN_NAME="@0@"'.format(name),
  ],
  dependencies: plugin_deps,
  install: true,
  install_dir: plugins_dir,
  install_mode: 'rw-r--r--',
)

# Localization and i18n setup
i18n.merge_file(
  input: configure_file(
    input: name + '.desktop.in.in',
    output: name + '.desktop.in',
    configuration: pluginconf,
  ),
  output: name + '.plugin',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: plugins_dir,
  type: 'desktop'
)

# Configure localization
conf_data = configuration_data()
conf_data.set_quoted('GETTEXT_PACKAGE', name)
conf_data.set_quoted('LOCALEDIR', localedir)

config_h = configure_file(
  input: 'config.h.in',
  output: 'config.h',
  configuration: conf_data,
)

# Subdirectory for localization
subdir('po')
