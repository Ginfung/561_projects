
struct vector_3D{
	float x;
	float y;
	float z;
};

struct face{
	int v1_index;
	int v2_index;
	int v3_index;
	int group_index;
	vector_3D norm;
};

struct face_group{
	char name[20];
	char mtlname[20];
	int mtl_index;
};

struct material{
	char name[20];
	float KaR, KaG, KaB;
	float KdR, KdG, KdB;
	float KsR, KsG, KsB;
	int N;
};

class objLoader{
public:
	char readl[100];
	char key;

	char mtlFileName[20];

	/*Data saved from obj file*/
	vector_3D vertex[50]; //save the vertex data
	int vertex_count;

	face facedata[50];//save the face data
	int face_count;

	face_group facegroupdata[20];
	int face_group_count;

	material materialdata[20]; //save the material data
	int material_count;

	objLoader(){}
	~objLoader(){}

	/*return the face group index whose name is given by the parameter*/
	int getGroupIndex(char *name);

	/*load the given OBJ file*/
	int loadObj(const char *objFile);

	/*load the material file while is indicated in the obj file*/
	int loadMtl();

	/*get the normal vector of each face*/
	void getFaceNorm();

private:
	/*after geting the material data, match the facegroup's material index*/
	void matchFaceGroupMtlIndex();

	/*cross product calculation*/
	vector_3D crossProduct(vector_3D A, vector_3D B);
};