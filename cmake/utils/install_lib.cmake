include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

function(install_lib target target_project_name c_standard)

	get_target_property(${target}_dir  ${target}   INCLUDE_DIRECTORIES)

	set(${target}_PUBLIC_HEADERS
			${${target}_dir}    
	)

	set_target_properties(${target}
		PROPERTIES 
			C_STANDARD          ${c_standard}
			C_STANDARD_REQUIRED ON
			C_EXTENSIONS        OFF
			# PUBLIC_HEADER       "${${target}_PUBLIC_HEADERS}"
			# EXPORT_NAME         ${target}
	)

	# Install ------------------------------------------------------------------------

	write_basic_package_version_file(${target}ConfigVersion.cmake
		VERSION       ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion 
	)

	## Target installation
	install(TARGETS   ${target}
		EXPORT        ${target_project_name}Targets
		ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${target}
		COMPONENT     library
	)

	## Target's cmake files: targets export
	install(EXPORT  ${target_project_name}Targets
		NAMESPACE   ${target_project_name}::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${target}
	)

	## Target's cmake files: config and version config for find_package()
	install(FILES   ${target}Config.cmake
					${CMAKE_CURRENT_BINARY_DIR}/${target}ConfigVersion.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${target}
	)


	diagnostic(${target})

endfunction(install_lib)
