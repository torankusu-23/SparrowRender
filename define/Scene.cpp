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
	//����ģ��װ��
	std::string RouteName = "../obj/" + name + "/";
	CString route = RouteName.c_str();
	std::cout <<"ģ���ļ�·����"<< route << std::endl;
	std::vector<CString> ModelName;	//�洢�ļ����б�
	
	WIN32_FIND_DATAA wfd;
	CString sPath = route + "*.obj";//����ָ��Ŀ¼�µ����и�ʽ���ļ���
	
	HANDLE hFile = FindFirstFile(sPath.GetBuffer(), &wfd);//����OBJ�ļ�
	if (INVALID_HANDLE_VALUE == hFile)
	{
		std::cout << "ģ��·�����Ϸ�!\n" << std::endl;
		return;
	}do {
		ModelName.push_back(wfd.cFileName);
		printf("%s\n", wfd.cFileName);
	} while (FindNextFile(hFile, &wfd));
	int m1 = ModelName.size();

	//��պ�ģ��װ��,ʹ��Ӳ����·�������Է���ģ��name�޸ĳɼ���ģʽ������Ҫ������¡�
	RouteName = "../obj/skybox4/";
	CString routeSky = RouteName.c_str();
	std::cout << "��պ��ļ�·����" << routeSky << std::endl;
	sPath = routeSky + "*.obj";//����ָ��Ŀ¼�µ����и�ʽ���ļ���
	hFile = FindFirstFile(sPath.GetBuffer(), &wfd);//����OBJ�ļ�
	if (INVALID_HANDLE_VALUE == hFile)
	{
		std::cout << "��պ�·�����Ϸ�!\n" << std::endl;
		return;
	}do {
		ModelName.push_back(wfd.cFileName);
		printf("%s\n", wfd.cFileName);
	} while (FindNextFile(hFile, &wfd));
	m = ModelName.size();

	int vertex = 0, face = 0;
	PhongShader* shader_phong = new PhongShader();
	SkyboxShader* shader_sky = new SkyboxShader();

	for (int i = 0; i < m1; i++)	//���ɶ���ģ��
	{
		model[i] = new Model(route + ModelName[i], false, true);
		vertex += model[i]->number_of_verts();
		face += model[i]->number_of_faces();
	}
	for (int i = m1; i < m; i++)	//������պ�ģ��
	{
		model[i] = new Model(routeSky + ModelName[i], true, false);
		vertex += model[i]->number_of_verts();
		face += model[i]->number_of_faces();
	}

	shader_phong->payload.camera_perp_matrix = perspective;	//camera_perp_matrix��;δ֪
	shader_phong->payload.camera = camera; 
	shader_sky->payload.camera_perp_matrix = perspective;
	shader_sky->payload.camera = camera;

	*shader_use = shader_phong;
	*shader_skybox = shader_sky;

	printf("scene name:%s\n", name.c_str());
	printf("model number:%d\n", m);
	printf("vertex:%d faces:%d\n", vertex, face);
}