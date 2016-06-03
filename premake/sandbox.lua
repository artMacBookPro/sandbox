
math.randomseed(1234)

local utils = require 'utils'

local use = UseModules or {}
sandbox_dir = _WORKING_DIR .. '/../..'

if SandboxRoot then
	sandbox_dir = _WORKING_DIR .. '/' .. SandboxRoot
end

flex_sdk_dir = nil
flascc_sdk_dir = nil

if not AndroidConfig then
	AndroidConfig = {}
end



use_network = use.Network

swf_size = SwfSize or { Width = 800, Height = 600}

solution( ProjectName )

	if os.is('emscripten') then
		toolset 'emcc'
	end

	configurations { 'Debug', 'Release' }

	if os.is('android') then
		android_abis {'armeabi'}
		android_stl('gnustl_static')
		android_activity(AndroidConfig.activity or 'com.sandboxgames.Activity')
		android_libs(path.getabsolute(path.join(sandbox_dir,'GHL/src/android_ghl')))
		android_libs(path.getabsolute(path.join(sandbox_dir,'platform/android/libs','sandbox_lib')))
		android_api_level(AndroidConfig.api_level or 9)
		android_target_api_level(AndroidConfig.target_api_level or 14)
		android_packagename( AndroidConfig.package or 'com.sandbox.example')
		android_toolchain( AndroidConfig.toolchain or '4.8' )
		android_screenorientation( AndroidConfig.screenorientation or 'landscape' )
		if AndroidConfig.keystore and AndroidConfig.keyalias then
			android_key_store(AndroidConfig.keystore)
			android_key_alias(AndroidConfig.keyalias)
		end
		android_packageversion( AndroidConfig.versioncode or 1)
		android_packageversionname( AndroidConfig.versionname or "1.0" )
		if use.AndroidGooglePlayService then
			android_modules_path( path.getabsolute(sandbox_dir) )
			local sdk_dir = assert(_OPTIONS['android-sdk-dir'])
			android_libs(path.join(sdk_dir,'extras/google/google_play_services/libproject','google-play-services_lib'))
			android_libs(path.join(sdk_dir,'extras/android/support/v4/android-support-v4.jar'))
			flags{ "C++11" }
			android_metadata {
				'com.google.android.gms.games.APP_ID=@string/app_id',
				'com.google.android.gms.version=@integer/google_play_services_version'
			}
		end
		if AndroidConfig.permissions then
			android_permissions( AndroidConfig.permissions )
		end
	end

	local hide_options = {
		'-fvisibility-inlines-hidden'
	}

	platform_dir = unknown
	local os_map = { 
		macosx = 'osx' 
	}
	local platform_id = os.get()


	if platform_id == 'flash' then
		flex_sdk_dir = _OPTIONS['flex-sdk-dir']
		flascc_sdk_dir = _OPTIONS['flascc-sdk-dir']
		if flex_sdk_dir == nil then
			error('must specify flex sdk dir')
		end
		if flascc_sdk_dir == nil then
			error('must specify crossbridge sdk dir')
		end
		gccprefix ( flascc_sdk_dir .. '/usr/bin/' )
		buildoptions {'-Wno-write-strings', '-Wno-trigraphs' }
	elseif platform_id ~= 'windows' then
		table.insert(hide_options,'-fvisibility=hidden')
	end

	buildoptions( hide_options )

	if platform_id == 'ios' then
		defines { 'GHL_PLATFORM_IOS' }
		xcodebuildsettings { 
			SDKROOT = 'iphoneos' , 
			CODE_SIGN_IDENTITY='iPhone Developer', 
			ARCHS='$(ARCHS_STANDARD)',
			IPHONEOS_DEPLOYMENT_TARGET='7.0'
		}
	end


	defines {'GHL_STATIC_LIB'}

	configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
    if platform_id == 'flash' then
    	buildoptions { '-g' }
    end

 
    configuration "Release"
         defines { "NDEBUG" }
    if platform_id == 'flash' then
    	buildoptions { '-O4' }
    elseif platform_id == 'emscripten' then
    	buildoptions { '-O2' }
    else
    	flags { "OptimizeSpeed" }    
   	end

	configuration {}

	platform_dir = os_map[platform_id] or platform_id

	

	local append_path = utils.append_path

	print('platform_dir:',platform_dir)



	local loc = _WORKING_DIR .. '/projects/' .. platform_dir
	location ( loc ) 
	
	objdir( _WORKING_DIR .. '/build/' .. platform_dir )

	libs_dir = _WORKING_DIR .. '/build/lib/' .. platform_dir

	language "C++"

	

	dofile('ghl.lua')

   	if use.Chipmunk then
		dofile('chipmunk.lua')
	end
	
	dofile('pugixml.lua')
    dofile('lua.lua')
  	dofile('yajl.lua')
  	dofile('tlsf.lua')
   	
   	if use.Freetype then
	   	dofile('freetype.lua')
	end

	if use.MyGUI then
		dofile('mygui.lua')
	end

	if use.Spine then
		dofile('spine-runtime.lua')
	end

	project 'Sandbox'

		kind 'StaticLib'

		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

		targetname ('Sandbox-' .. platform_dir)

		
		files {
			sandbox_dir .. '/include/**.h',
			sandbox_dir .. '/sandbox/*.h',
			sandbox_dir .. '/sandbox/*.cpp',
			sandbox_dir .. '/sandbox/luabind/**.h',
			sandbox_dir .. '/sandbox/luabind/**.cpp',
			sandbox_dir .. '/sandbox/meta/**.h',
			sandbox_dir .. '/sandbox/meta/**.cpp',
			sandbox_dir .. '/sandbox/json/**.h',
			sandbox_dir .. '/sandbox/json/**.cpp',
			sandbox_dir .. '/sandbox/skelet/**.h',
			sandbox_dir .. '/sandbox/skelet/**.cpp',
			sandbox_dir .. '/sandbox/utils/**.cpp',
			sandbox_dir .. '/sandbox/utils/**.c',
			sandbox_dir .. '/sandbox/utils/**.h',
		}

		sysincludedirs {
			sandbox_dir .. '/GHL/include',
			sandbox_dir .. '/include',
			
			sandbox_dir .. '/external/freetype/include',
			sandbox_dir .. '/external/yajl/src/api',
			sandbox_dir .. '/external/pugixml/src',
			sandbox_dir .. '/external/tlsf'
		}

		includedirs {
			sandbox_dir .. '/sandbox',
		}

		if use.MyGUI then
			files {
				sandbox_dir .. '/sandbox/mygui/**.h',
				sandbox_dir .. '/sandbox/mygui/**.cpp',
			}
			includedirs {
				sandbox_dir .. '/external/MyGUI/MyGUIEngine/include',
			}
			if os.is('windows') then
				defines 'MYGUI_CONFIG_INCLUDE="sb_mygui_config.h"'
			else
				defines 'MYGUI_CONFIG_INCLUDE="<sb_mygui_config.h>"'
			end
			defines 'SB_USE_MYGUI'
		end

		if use.Spine then
			files {
				sandbox_dir .. '/sandbox/spine/**.h',
				sandbox_dir .. '/sandbox/spine/**.cpp',
			}
			sysincludedirs {
				sandbox_dir .. '/external/spine-runtime-c/include',
			}
			defines 'SB_USE_SPINE'
		end

		if use.Chipmunk then
			files {
				sandbox_dir .. '/sandbox/chipmunk/*.cpp',
				sandbox_dir .. '/sandbox/chipmunk/*.h',
			}
		end

		if use_network then
			files {
				sandbox_dir .. '/sandbox/net/**.cpp',
				sandbox_dir .. '/sandbox/net/**.h',
			}
			defines 'SB_USE_NETWORK'
		end


		configuration "Debug"
   			targetsuffix "-debug"
   			defines 'SB_DEBUG'

	project( ProjectName )

		kind 'WindowedApp'

		targetdir (_WORKING_DIR .. '/bin/' .. platform_dir)

		libdirs { _WORKING_DIR .. '/lib/' .. platform_dir }

		local libs_postfix = ''
		if os.is('macosx') then
			libs_postfix = '-OSX'
		end

		links( {
			'Sandbox', 
			'pugixml',
			'lua', 
			'yajl',
			'tlsf',
			'GHL'
		} )

		if use.MyGUI then
			links { 'MyGUI' }
			includedirs { sandbox_dir .. '/external/MyGUI/MyGUIEngine/include' }
			if os.is('windows') then
				defines 'MYGUI_CONFIG_INCLUDE="mygui/sb_mygui_config.h"'
			else
				defines 'MYGUI_CONFIG_INCLUDE="<sb_mygui_config.h>"'
			end
			defines 'SB_USE_MYGUI'
		end
		if use.Freetype then
			links { 'freetype' }
		end
		if use.Chipmunk then
			links { 'chipmunk' }
			includedirs { sandbox_dir .. '/external/chipmunk/include' }
		end
		if use.Spine then
			links { 'spine-runtime' }
			defines 'SB_USE_SPINE'
		end
		if os.is('ios') then
			files { sandbox_dir .. '/platform/ios/main.mm',
					sandbox_dir .. '/platform/ios/*.cpp',
					sandbox_dir .. '/platform/ios/*.h' }
			links {
				'Foundation.framework', 
				'QuartzCore.framework', 
				'AVFoundation.framework', 
				'UIKit.framework',  
				'OpenGLES.framework', 
				'OpenAL.framework',
				'AudioToolbox.framework',
				'CoreMotion.framework' }
		elseif os.is('macosx') then
			files { sandbox_dir .. '/platform/osx/main.mm',
					sandbox_dir .. '/platform/osx/*.cpp',
					sandbox_dir .. '/platform/osx/*.h' }
			links { 
				'OpenGL.framework', 
				'OpenAL.framework',
				'Cocoa.framework',
				'AudioToolbox.framework' }
		elseif os.is('flash') then
			files { sandbox_dir .. '/platform/flash/*.cpp' }
			links {
				'AS3++',
				'Flash++'
			}
		elseif os.is('windows') then
			files { sandbox_dir .. '/platform/windows/*.cpp' }
			links {
				'OpenGL32',
				'WinMM'
			}
			if use_network then
				links { 'Winhttp', }
			end
		elseif os.is('android') then
			files { sandbox_dir .. '/platform/android/main.cpp',
					sandbox_dir .. '/platform/android/sb_android_extension.cpp' }
			links {
				'android',
				'log',
				'EGL',
				'OpenSLES',
				'GLESv1_CM',
				'GLESv2'
			}
			if use.AndroidGooglePlayService then
				files { sandbox_dir .. '/platform/android/gps_extension.cpp' }
				includedirs { sandbox_dir .. '/external/gpg-cpp-sdk/android/include' }
				android_ndk_static_libs {
					'gpg-1',
				}
				android_ndk_modules {
					'external/gpg-cpp-sdk/android'
				}
			end
		elseif os.is('emscripten') then
			files { sandbox_dir .. '/platform/emscripten/*.cpp' }
			links {
				'SDL'
			}
			linkoptions  {
				'-s USE_SDL=2',
				'-s FULL_ES2=1'
			}
		end


		-- files {
		-- 	_WORKING_DIR .. '/src/**.h',
		-- 	_WORKING_DIR .. '/src/**.cpp'
		-- }

		-- resourcefolders {
		-- 	_WORKING_DIR .. '/data'
		-- }

		-- if os.is('macosx') then
		-- 	files { 
		-- 		_WORKING_DIR .. '/projects/osx/main.mm',
		-- 		_WORKING_DIR .. '/projects/osx/' .. ProjectName .. '_Mac-Info.plist'
		-- 	}
		-- 	prebuildcommands { "touch " .. path.getabsolute( _WORKING_DIR .. '/data') }
		-- elseif os.is('ios') then
		-- 	files { 
		-- 		_WORKING_DIR .. '/projects/ios/main.mm',
		-- 		_WORKING_DIR .. '/projects/ios/'..ProjectName..'_iOS-Info.plist',
		-- 		_WORKING_DIR .. '/projects/ios/Default@2x.png',
		-- 		_WORKING_DIR .. '/projects/ios/Default-568h@2x.png',
		-- 	}
		-- 	prebuildcommands { "touch " .. path.getabsolute(_WORKING_DIR .. '/data') }
		-- elseif os.is('flash') then
		-- 	targetextension( '.swf' )
		-- 	files { 
		-- 		_WORKING_DIR .. '/projects/flash/main.cpp',
		-- 	}
		-- 	prelinkcommands { 
		-- 		'rm -f ' .. path.getabsolute( _WORKING_DIR .. '/bin' ) .. '/flash/' .. ProjectName .. '.swf',
		-- 		flascc_sdk_dir .. '/usr/bin/genfs --type=embed ' .. path.getabsolute('data') .. ' ' .. path.getabsolute(_WORKING_DIR ..'/build/' .. platform_dir .. '/data') ,
		-- 		[[java $(JVMARGS) -jar ]] .. flascc_sdk_dir .. [[/usr/lib/asc2.jar -merge -md \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/builtin.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/playerglobal.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/BinaryData.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/ISpecialFile.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/IBackingStore.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/IVFS.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/InMemoryBackingStore.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/PlayerKernel.abc \
		-- 			 ]]..flascc_sdk_dir..[[/usr/share/LSOBackingStore.as \
		-- 			 ]]..path.getabsolute( _WORKING_DIR .. '/' .. ghl_src ) .. [[/flash/Console.as \
		-- 			 ]]..path.getabsolute('build/' .. platform_dir .. '/data')..[[*.as -outdir ]]..path.getabsolute(_WORKING_DIR .. '/build/' .. platform_dir )..[[ -out Console
		-- 		]]
		-- 	}


		-- 	linkoptions  {
		-- 			'-jvmopt="-Xmx4096M"',
		-- 			'-emit-swf',
		-- 			'-swf-size=1024x768',
		-- 			'-flto-api=exports.txt',
		-- 			flascc_sdk_dir .. '/usr/lib/AlcVFSZip.abc',
		-- 			'-symbol-abc=' .. path.getabsolute(_WORKING_DIR .. '/build') .. '/' .. platform_dir .. '/Console.abc'
		-- 		}
			
			
		-- end

		sysincludedirs {
			sandbox_dir .. '/GHL/include',
			sandbox_dir .. '/include',
			sandbox_dir .. '/sandbox',
		}


		if use_network then
			defines 'SB_USE_NETWORK'
		end

		linkoptions( hide_options )

		configuration "Release"
			if os.is('flash') then
				linkoptions  {
					'-O4'
				}
			end
		
		configuration "Debug"
   			defines 'SB_DEBUG'
   			if os.is('flash') then
				linkoptions  {
					'-g'
				}
			end
		configuration {}