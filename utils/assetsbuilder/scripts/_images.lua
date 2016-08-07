
local _M = {}

_M.assets_rules = {}
_M.use_variants = {}

function _M.init_rules(rules)
	rules.images = {}
	rules.atlases = {}
	rules.premultiply_images = {}
end

function _M.assets_rules.set_alpha_file_format( func )

	_M.alpha_file_format = func

end

local atlas = require '_atlas'

local function load_images( dir  )
	local fullpath = path.join(dir,'images.lua')
	local ctx = { textures = {} , images = {} , animations = {}}
	local sandbox = { textures = ctx.textures }
	function sandbox.load_group( name )
		return {path=name, data=load_images( path.join(dir,name) ) }
	end
	function sandbox._textures( textures )
		for k,v in pairs(textures) do
			local filename = path.join(dir,k .. '.' .. assert(v.type,'not found type for ' .. k .. ' at ' .. dir))
			local tex = assert(application:check_texture(filename),'failed check texture ' .. filename)
			v.texture_info = tex

			local premultiplied = v.premultiplied
			if premultiplied then
				get_rules().copy_files[filename]=true
			else
				local pm = assert(get_rules().premultiply_images)
				local texobj = { dst = filename }
				if v.aname then
					texobj.aname = path.join(dir,v.aname)
					texobj.dst = path.join(dir,k .. '.png')
					v.type = 'png'
				end
				pm[filename] = texobj 
				print('premultiply image',filename)
				
				v.premultiplied = true
				v._need_premultiply = true
			end
			assert(not get_rules().dest_files[filename],'conflict rules for file ' .. filename)
			get_rules().dest_files[filename]=filename
			v._name = k
			v._path = dir
			ctx.textures[k] = v

			if _M.use_variants then
				local override_base = false
				for vk,vv in pairs(_M.use_variants) do
					if vv.override_base then
						override_base = true
					end
					local vname = path.join(dir,k .. vk.. '.' .. v.type)
					local vtex = application:check_texture(vname)
					if vtex then
						if premultiplied then
							get_rules().copy_files[vname]=true
							print('copy variant',vname)
						else
							get_rules().premultiply_images[vname] = vname
							print('premultiply variant',vname)
						end
						assert(not get_rules().dest_files[vname],'conflict rules for file ',vname)
						get_rules().dest_files[vname]=vname

						if override_base then
							get_rules().copy_files[filename] = nil
							get_rules().premultiply_images[filename] = nil
							get_rules().dest_files[filename]=nil
						end
					else
						print('not found varian for',filename)
					end
				end
			end
		end
	end
	function sandbox._images( images )
		for k,v in pairs(images) do
			local tex = assert(v[1],'texture empty for ' .. k .. ' at ' .. dir)
			v.texture = v[1]
			ctx.images[k] = v
		end
	end
	function sandbox._animations( animations )
		for k,v in pairs(animations) do
			ctx.animations[k] = v
		end
	end
	local data = load_sandbox(fullpath,sandbox,{})
	data._images = ctx.images
	data._animations = ctx.animations
	data._textures = ctx.textures
	return data
end

local function find_images_folder( root , path )
	local first,other = string.match(path,'([^/]+)/(.+)')
	local prefix = ''
	if not first then
		--print('find_images_folder:',path)
		return root[path],root[path],''
	end
	local point = root[first]
	for i in string.gmatch(other, "[^/]+") do
		if prefix~='' then
			prefix = prefix .. '/'
		end
		prefix = prefix .. i
		--print('find_images_folder:',first,i)
  		point = point[i].data
  		if not point then
  			return nil
  		end
	end 
	return point,root[first],prefix
end

local function find_textures( from, mask )
	local res = r or {}
	local wmask = path.wildcards(path.join(from,mask))
	local function match( name )
		return name:match(wmask)
	end 
	local function filter_group( g )
		local t = g._textures 
		for _,v in pairs(t) do
			if match(v._path .. '/' .. v._name) then
				--print('filtered:',v._path .. '/' .. v._name)
				table.insert(res,v)
			end
		end
		local gg = {}
		for k,v in pairs(g) do
			gg[k]=v
		end
		gg._images = nil
		gg._textures = nil
		gg._animations = nil
		for k,v in pairs(gg) do
			filter_group(v.data)
		end
	end
	filter_group(assert(find_images_folder(get_rules().images,from),"not found images '" .. from .."'"))
	return res
end

function _M.assets_rules.build_images( p )
	local i = assert(get_rules().images)
	i[p]=load_images(p)
	get_rules().dest_files[path.join(p,'images.lua')]=true
end



function _M.assets_rules.build_atlas( from, mask , name,  w, h )
	local i = find_textures( from, mask )
	local a = atlas.Atlas.new(w,h)
	
	local g,root,atlas_prefix = assert(find_images_folder(get_rules().images,from))
	local atlas_name = name
	if atlas_prefix ~='' then
		atlas_name = string.gsub(atlas_prefix,'/','_') .. '_' .. atlas_name
	end
	a.name = atlas_name
	for _,v in ipairs(i) do
		local path = path.join(v._path,v._name .. '.' .. v.type)
		a:add_image( {width=v.texture_info.width+2, height=v.texture_info.height+2, src = v, 
			premultiply = v._need_premultiply} )
		print('put to atlas:',path,v._need_premultiply and true or false)
		get_rules().dest_files[path] = nil
		get_rules().premultiply_images[path] = nil
		v._atlas = a
	end
	a:build()
	--a:dump()
	for _,v in ipairs(a.images) do
		v.src._placed_to = {v.place_to[1] + 1, v.place_to[2] + 1 }
	end
	a.path = path.join(from,name)
	local override_base = false

	
	if _M.use_variants then
		
		for vk,vv in pairs(_M.use_variants) do
			if vv.override_base then
				override_base = true
			end
			for _,v in ipairs(i) do
				local opath = path.join(v._path,v._name .. '.' .. v.type)
				local vname = path.join(v._path,v._name .. vk .. '.' .. v.type)
				local vtex = application:check_texture(vname)
				if vtex then
					if vtex.width ~= ( v.texture_info.width * vv.scale) then
						error('texture width must be some ' .. atlas_name .. ' ' .. v._path .. '/' .. v._name)
					end
					if vtex.height ~= ( v.texture_info.height * vv.scale) then
						error('texture height must be some ' .. atlas_name .. ' ' .. v._path .. '/' .. v._name)
					end
					get_rules().dest_files[vname] = nil
					get_rules().premultiply_images[vname] = nil

					get_rules().dest_files[a.path .. vk .. '.png']=true
				else
					error('for build atlas need all resolutions ' .. atlas_name .. ' ' .. v._path .. '/' .. v._name )
				end
			end
			
		end
	end

	local atex = { type='png', premultiplied = true, smooth=true, _path = path.join(from) , _name = name }
	g._textures[name] = atex
	function a:apply(  )
		print('build atlas ' .. self.name .. ' with ' .. tostring(#self.images) .. ' textures')
		if not override_base then
			local img = TextureData( self.with, self.height )
			for _,v in ipairs(self.images) do
				--print(v.src._path,v.src._name)
				local tpath = path.join(v.src._path,v.src._name .. '.' .. v.src.type)
				local i = assert(application:load_texture(tpath),'failed load texture ' .. tpath)
				if v.src.aname  then
					local apath = path.join(v.src._path,v.src.aname )
					local ai = assert(application:load_texture(apath),'failed load alpha texture ' .. apath)
					i:SetAlpha(ai)
				end
				if v.premultiply then
					i:PremultiplyAlpha()
				end
				img:Place(v.place_to[1]+1,v.place_to[2]+1,i)
			end
			application:store_texture(self.path .. '.png' ,img)
		end
		if _M.use_variants then
			for vk,vv in pairs(_M.use_variants) do
				local img = TextureData( self.width * vv.scale , self.height * vv.scale )
				for _,v in ipairs(self.images) do
					--print(v.src._path,v.src._name)
					local tpath = path.join(v.src._path,v.src._name .. vk .. '.' .. v.src.type)
					local i = assert(application:load_texture(tpath),'failed load texture ' .. tpath)
					if v.src.aname  then
						local apath = path.join(v.src._path,v.src.aname .. vk )
						local ai = assert(application:load_texture(apath),'failed load alpha texture ' .. apath)
						i:SetAlpha(ai)
					end
					if v.premultiply then
						i:PremultiplyAlpha()
					end
					img:Place((v.place_to[1]+1)*vv.scale,(v.place_to[2]+1)*vv.scale,i)
				end
				application:store_texture(self.path .. vk .. '.png',img)
			end
		end
	end
	local atlases = assert(get_rules().atlases)
	if not override_base then
		get_rules().dest_files[a.path .. '.png']=true
	end
	table.insert(atlases,a)
end

local function apply_images( dir, data )
	local fn = path.join(dir,'images.lua')
	print('generate',fn)
	local file = assert(io.open ( path.join(dst_path,fn) , 'w'))
	
	local function f(name,val,def)
		val = val or def
		local fmt = "%s=%s,"
		if type(val) == 'boolean' then
			val = iif(val,'true','false')
		elseif type(val) == 'string' then
			fmt = "%s='%s',"
		elseif type(val)=='table' then
			val = '{' .. table.concat(val,',') .. '}'
		end
		if not val then
			return ""
		end
		return string.format(fmt,name,tostring(val))
	end
	local function p( ... )
		local args = table.pack(...)
		table.insert(args,'\n')
		file:write(table.unpack(args))
	end
	local function x( offset, ... )
		local fmt = ''
		for _=1,offset do
			fmt = fmt .. '  '
		end
		local args = table.pack(...)
		table.insert(args,'\n')
		file:write(fmt,table.unpack(args))
	end
	local function expand_group( group , func , prefix, name )
		local g = {}
		for k,v in pairs(group.data) do
			g[k] = v
		end
		g._textures = nil
		g._images = nil
		g._animations = nil
		local path = group.path
		if path ~= '' then
			path = path .. '/'
		end
		
		for k,v in pairs(g) do
			func(prefix .. k .. '_', v.data[name],path .. v.path)
			expand_group(v,func,prefix..k..'_',name)
		end
	end
	local function print_textures(  prefix, textures , path )
		for k,v in pairs(textures) do
			if not v._atlas then
				v._mapped_name = prefix .. k
				local p =''
				if path and path ~= '' then
					p = "'" .. path .. '/' .. k .. "',"
				end
				x(1,prefix .. k,' = {',
					p,
					f('type',v.type),
					f('smooth',v.smooth,false),
					f('tiled',v.tiled,false),
					f('premultiplied',v.premultiplied,false),
					'},')
			end
		end
	end
	p("_textures {")
	print_textures('',data._textures)
	expand_group({path='',data=data},print_textures,'','_textures')
	p("}")
	local function print_images( prefix, images )
		for k,v in pairs(images) do
			local texture = nil
			if v.texture._atlas then
				texture = string.format('textures.%s,',v.texture._atlas.name)
				if v.rect then
					v.rect[1] = v.rect[1] + v.texture._placed_to[1]
					v.rect[2] = v.rect[2] + v.texture._placed_to[2]
				else
					v.rect = {}
					v.rect[1] = v.texture._placed_to[1]
					v.rect[2] = v.texture._placed_to[2]
					v.rect[3] = v.texture.texture_info.width
					v.rect[4] = v.texture.texture_info.height
				end
			else
				texture = string.format('textures.%s,',v.texture._mapped_name)
			end
			x(1,prefix..k,'={',
					texture,
					f('rect',v.rect),
					f('hotspot',v.hotspot),
					f('size',v.size),
					f('offsets',v.offsets),
					'},')
		end
	end 
	p("_images {")
	print_images('',data._images)
	expand_group({path='',data=data},print_images,'','_images')
	p("}")
	local function print_animations( prefix , animations )
		for k,v in pairs(animations) do
			x(1,prefix..k,'={',
				string.format("'%s%s',%d,%d,",prefix,v[1],v[2],v[3]),
				f('speed',v.speed),
				f('frameset',v.frameset),
				'},')
		end
	end 
	p("_animations {")
	print_animations('',data._animations)
	expand_group({path='',data=data},print_animations,'','_animations')
	p("}")
	local groups = {}
	for k,v in pairs(data) do
		groups[k]=v
	end
	groups._images = nil
	groups._textures = nil
	groups._animations = nil
	local function expand_group_names( ident, prefix, group )
		local prefix = prefix .. '_'
		for k,v in pairs(group._images) do
			x(ident,k,'=images.',prefix..k,',')
		end
		for k,v in pairs(group._animations) do
			x(ident,k,'=animations.',prefix..k,',')
		end
		local g = {}
		for k,v in pairs(group) do
			g[k]=v
		end
		g._textures = nil
		g._images = nil
		g._animations = nil
		for k,v in pairs(g) do
			x(ident,k,' = {')
			expand_group_names(ident+1,prefix .. k,v.data)
			x(ident,'},')
		end
	end
	for k,v in pairs(groups) do
		assert(type(v)=='table','unknown field ' .. k)
		p(k," = {" )
		expand_group_names(1,k,v.data)
		p("}")
	end
	io.close(file)
	for k,v in pairs(groups) do
		--apply_images(path.join(dir,v.path),v.data)
	end
end

local function do_premultiply_file( src, dstconf  )
	local conf = { dst = dstconf }
	if type(dstconf) == 'table' then
		conf = dstconf
	end

	if update_only then
		if not os.check_file_new(path.join(src_path,src),path.join(dst_path,conf.dst)) then
			--print('skip not new')
			return true
		end
	end
	local t = assert(application:load_texture(src))
	local aimage = conf.aname
	if not aimage and type(dstconf) == 'string' and _M.alpha_file_format then
		aimage = _M.alpha_file_format(src)
	end
	if aimage then
		if aimage then
			local at = application:load_texture(aimage)
			if at then
				t:SetAlpha(at)
			end
		end
	end
	t:PremultiplyAlpha()
	return application:store_texture(conf.dst,t)
end

function _M.filter_files( filelist )
	local map = {}
	for k,v in ipairs(filelist) do
		map[v] = v
	end

end

function _M.apply( rules )
	print('images apply:',rules)
	local images = rules.images or {}
	for k,v in pairs(images) do
		apply_images(k,v)
	end
	local atlases = rules.atlases or {}
	for _,v in ipairs(atlases) do
		v:apply()
	end

	local pmi = rules.premultiply_images or {}
	print('premultiply images')
	for k,v in pairs(pmi) do
		if v then
			--print('premultiply',k)
			if type(v) == 'table' then
				assert(do_premultiply_file(k,v),'failed store texture to ' .. v.dst)
			elseif type(v) == 'string' then
				assert(do_premultiply_file(k,v),'failed store texture to ' .. v)
			else
				assert(do_premultiply_file(k,k),'failed store texture to ' .. k)
			end
		end
	end
end

return _M