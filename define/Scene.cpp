#include "../head/Scene.h"
#include "../head/Win.h"
#include <vector>
#include <string>
#include <atlstr.h>
Image* texture_from_file(const char* file_name)
{
	Image* texture = new Image();
	texture->read_tga_file(file_name);
	texture->flip_vertically();
	return texture;
}

cubemap_t* cubemap_from_files(const char* positive_x, const char* negative_x,
	const char* positive_y, const char* negative_y,
	const char* positive_z, const char* negative_z)
{
	cubemap_t* cubemap = new cubemap_t();
	cubemap->faces[0] = texture_from_file(positive_x);
	cubemap->faces[1] = texture_from_file(negative_x);
	cubemap->faces[2] = texture_from_file(positive_y);
	cubemap->faces[3] = texture_from_file(negative_y);
	cubemap->faces[4] = texture_from_file(positive_z);
	cubemap->faces[5] = texture_from_file(negative_z);
	return cubemap;
}

void load_ibl_map(payload_t& p, const char* env_path)
{
	int i, j;
	iblmap_t* iblmap = new iblmap_t();
	const char* faces[6] = { "px", "nx", "py", "ny", "pz", "nz" };
	char paths[6][256];

	iblmap->mip_levels = 10;

	/* diffuse environment map */
	for (j = 0; j < 6; j++) {
		sprintf_s(paths[j], "%s/i_%s.tga", env_path, faces[j]);
	}
	iblmap->irradiance_map = cubemap_from_files(paths[0], paths[1], paths[2],
		paths[3], paths[4], paths[5]);

	/* specular environment maps */
	for (i = 0; i < iblmap->mip_levels; i++) {
		for (j = 0; j < 6; j++) {
			sprintf_s(paths[j], "%s/m%d_%s.tga", env_path, i, faces[j]);
		}
		iblmap->prefilter_maps[i] = cubemap_from_files(paths[0], paths[1],
			paths[2], paths[3], paths[4], paths[5]);
	}

	/* brdf lookup texture */
	iblmap->brdf_lut = texture_from_file("../obj/common/BRDF_LUT.tga");

	p.iblmap = iblmap;

}

void build_yayi_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 7;
	const char* modelname[] = {
		"../obj/yayi/yayiface.obj",
		"../obj/yayi/yayibody.obj",
		"../obj/yayi/yayihair.obj",
		"../obj/yayi/yayiarmour1.obj",
		"../obj/yayi/yayiarmour2.obj",
		"../obj/yayi/yayidecoration.obj",
		"../obj/yayi/yayisword.obj"
	};

	int vertex = 0, face = 0;
	const char* scene_name = "yayi";
	PhongShader* shader_phong = new PhongShader();

	for (int i = 0; i < m; i++)
	{
		model[i] = new Model(modelname[i], false, true);
		vertex += model[i]->number_of_verts();
		face += model[i]->number_of_faces();
	}

	shader_phong->payload.camera_perp_matrix = perspective;
	shader_phong->payload.camera = camera;

	*shader_use = shader_phong;
	*shader_skybox = NULL;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}

void build_helmet_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 2;
	const char* modelname[] =
	{
		"../obj/helmet/helmet.obj",
		"../obj/skybox4/box.obj",
	};

	//PBRShader* shader_pbr = new PBRShader();
	SkyboxShader* shader_sky = new SkyboxShader();

	int vertex = 0, face = 0;
	const char* scene_name = "helmet";
	model[0] = new Model(modelname[0], 0, 0); vertex += model[0]->number_of_verts(); face += model[0]->number_of_faces();
	model[1] = new Model(modelname[1], 1, 0); vertex += model[1]->number_of_verts(); face += model[1]->number_of_faces();

	//shader_pbr->payload.camera_perp_matrix = perspective;
	//shader_pbr->payload.camera = camera;
	shader_sky->payload.camera_perp_matrix = perspective;
	shader_sky->payload.camera = camera;

	//load_ibl_map(shader_pbr->payload, "../obj/common2");

	//*shader_use = shader_pbr;
	*shader_skybox = shader_sky;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}

void build_gun_scene(Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	m = 2;
	const char* modelname[] =
	{
		"../obj/gun/Cerberus.obj",
		"../obj/skybox4/box.obj",
	};

	//PBRShader* shader_pbr = new PBRShader();
	SkyboxShader* shader_sky = new SkyboxShader();

	int vertex = 0, face = 0;
	const char* scene_name = "gun";
	model[0] = new Model(modelname[0], 0, 0); vertex += model[0]->number_of_verts(); face += model[0]->number_of_faces();
	model[1] = new Model(modelname[1], 1, 0); vertex += model[1]->number_of_verts(); face += model[1]->number_of_faces();

	//shader_pbr->payload.camera_perp_matrix = perspective;
	//shader_pbr->payload.camera = camera;
	shader_sky->payload.camera_perp_matrix = perspective;
	shader_sky->payload.camera = camera;

	//load_ibl_map(shader_pbr->payload, "../obj/common2");

	//*shader_use = shader_pbr;
	*shader_skybox = shader_sky;

	printf("scene name:%s\n", scene_name);
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}

void build_scene(std::string name, Model** model, int& m, IShader** shader_use, IShader** shader_skybox, mat4 perspective, Camera* camera)
{
	//对象模型装载
	std::string RouteName = "../obj/" + name + "/";
	CString route = RouteName.c_str();
	std::cout <<"模型文件路径："<< route << std::endl;
	std::vector<CString> ModelName;	//存储文件名列表
	
	WIN32_FIND_DATAA wfd;
	CString sPath = route + "*.obj";//查找指定目录下的所有格式的文件。
	
	HANDLE hFile = FindFirstFile(sPath.GetBuffer(), &wfd);//查找OBJ文件
	if (INVALID_HANDLE_VALUE == hFile)
	{
		std::cout << "模型路径不合法!\n" << std::endl;
		return;
	}do {
		ModelName.push_back(wfd.cFileName);
		printf("%s\n", wfd.cFileName);
	} while (FindNextFile(hFile, &wfd));
	int m1 = ModelName.size();

	//天空盒模型装载,使用硬编码路径，可以仿照模型name修改成兼容模式，在需要的情况下。
	RouteName = "../obj/skybox4/";
	CString routeSky = RouteName.c_str();
	std::cout << "天空盒文件路径：" << routeSky << std::endl;
	sPath = routeSky + "*.obj";//查找指定目录下的所有格式的文件。
	hFile = FindFirstFile(sPath.GetBuffer(), &wfd);//查找OBJ文件
	if (INVALID_HANDLE_VALUE == hFile)
	{
		std::cout << "天空盒路径不合法!\n" << std::endl;
		return;
	}do {
		ModelName.push_back(wfd.cFileName);
		printf("%s\n", wfd.cFileName);
	} while (FindNextFile(hFile, &wfd));
	m = ModelName.size();

	int vertex = 0, face = 0;
	PhongShader* shader_phong = new PhongShader();
	SkyboxShader* shader_sky = new SkyboxShader();

	for (int i = 0; i < m1; i++)	//生成对象模型
	{
		model[i] = new Model(route + ModelName[i], false, true);
		vertex += model[i]->number_of_verts();
		face += model[i]->number_of_faces();
	}
	for (int i = m1; i < m; i++)	//生成天空盒模型
	{
		model[i] = new Model(routeSky + ModelName[i], true, false);
		vertex += model[i]->number_of_verts();
		face += model[i]->number_of_faces();
	}

	shader_phong->payload.camera_perp_matrix = perspective;	//camera_perp_matrix用途未知
	shader_phong->payload.camera = camera; 
	shader_sky->payload.camera_perp_matrix = perspective;
	shader_sky->payload.camera = camera;

	*shader_use = shader_phong;
	*shader_skybox = shader_sky;

	printf("scene name:%s\n", name.c_str());
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}