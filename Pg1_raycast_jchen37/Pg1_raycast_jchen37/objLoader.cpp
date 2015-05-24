#include "stdafx.h"
#include "objLoader.h"


#pragma warning(disable: 4996)

/*return the face group index whose name is given by the parameter*/
int objLoader::getGroupIndex(char *name){
	for (int i = 0; i < face_group_count; i++){
		if (!strcmp(name, facegroupdata[i].name))
			return i;
	}
	return 0;
}

int objLoader::loadObj(const char *objFile){
	//open the OBJ file
	FILE *fp;
	if ((fp = fopen(objFile, "r")) == NULL){
		std::printf("cannot open the obj file\n");
		return -1;
	}

	vertex_count = 0;
	face_count = 0;
	face_group_count = 0;

	//parase the obj file line by line
	while ((readl[0] = fgetc(fp)) != EOF){
		/*Begin parasing the obj file*/
	L:switch (readl[0]){
	case '#': //comments
		fgets(readl, 100, fp);
		break;
	case 'm': //"mtllib" tag found. save the mtl file path and name
		fscanf(fp, "tllib %s\n", &mtlFileName);
		break;
	case 'v': //'v' tag found. save the vertex data in an array
		fscanf(fp, "%f %f %f\n", &vertex[vertex_count].x, &vertex[vertex_count].y, &vertex[vertex_count].z);
		vertex_count++;
		break;
	case 'g': //'g' OR 'group' tag found. need to distinguish them
		if (fgetc(fp) != 'r'){ //'g' tag found. save the group information, including the face infomation
			fscanf(fp, "%s\n", &facegroupdata[face_group_count].name); //save group name
			face_group_count++;
			break;
		}
		else{//'group' tag found. save the group material information 
			char temp[20] = { ' ' };
			char temp2[20] = { ' ' };
			fscanf(fp, "oup %s\n", &temp);
			fscanf(fp, "usemtl %s\n", &temp2);
			int k = getGroupIndex(temp2);
			memcpy(facegroupdata[k].mtlname, temp2, sizeof(temp2));
			break;
		}
		break;
	case 's':
		fgets(readl, 100, fp);
		break;
	case 'f': //'f' tag found. save the face information
		fscanf(fp, "%d %d %d\n", &facedata[face_count].v1_index, &facedata[face_count].v2_index, &facedata[face_count].v3_index);
		facedata[face_count].v1_index--;
		facedata[face_count].v2_index--;
		facedata[face_count].v3_index--;

		facedata[face_count].group_index = face_group_count - 1;
		face_count++;
		break;
	default:
		fgets(readl, 100, fp);
	}
	}
	fclose(fp);
	loadMtl();
	getFaceNorm();

	return 0;
}

int objLoader::loadMtl(){
	//open the OBJ file
	fstream in;
	in.open(this->mtlFileName, ios::in);
	if (!in.is_open()){
		std::printf("cannot open the mtl file\n");
		return -1;
	}

	material_count = 0;
	/*begin parasing the material file*/
	while (!in.eof()){
		char initial;
		in >> initial; //read the first letter for each line
		switch (initial){
		case '#': //"#" tag found. Ignore the whole line
			in.getline(readl, 100);
			break;
		case 'n': //"newmtl" tag found. Save the material data
			in >> readl >> materialdata[material_count].name; //name
			in >> readl >> materialdata[material_count].KaR >> materialdata[material_count].KaG >> materialdata[material_count].KaB; //Ka
			in >> readl >> materialdata[material_count].KdR >> materialdata[material_count].KdG >> materialdata[material_count].KdB; //Kd
			in >> readl >> materialdata[material_count].KsR >> materialdata[material_count].KsG >> materialdata[material_count].KsB; //Ks
			in >> initial; //detect the next letter
			if (initial == 'N') //"N" tag found. Save it
				in >> materialdata[material_count].N;
			else
				materialdata[material_count].N = 1; //set default value of N is 1
			material_count++;
			break;
		default:
			in.getline(readl, 100);
			break;
		}
	}
	in.close();
	matchFaceGroupMtlIndex();
	return 0;
}

/*after geting the material data, match the facegroup's material index*/
void objLoader::matchFaceGroupMtlIndex(){
	for (int i = 0; i < this->face_group_count; i++) //for each face group
	for (int j = 0; j < this->material_count; j++) // for each material
	if (!strcmp(this->facegroupdata[i].mtlname, this->materialdata[j].name)){
		this->facegroupdata[i].mtl_index = j;
		break;
	}

	return;
}

/*get the normal vector of each face*/
void objLoader::getFaceNorm(){
	for (int i = 0; i < this->face_count; i++){ //for each face
		vector_3D norm_result;
		// V_AB = v2_x-v1_x, v2_y-v1_y, v2_z-v1_z
		vector_3D AB;
		AB.x = vertex[facedata[i].v2_index].x - vertex[facedata[i].v1_index].x;
		AB.y = vertex[facedata[i].v2_index].y - vertex[facedata[i].v1_index].y;
		AB.z = vertex[facedata[i].v2_index].z - vertex[facedata[i].v1_index].z;
		// V_AC = v3_x-v1_x, v3_y-v1_y, v3_z-v1_z
		vector_3D AC;
		AC.x = vertex[facedata[i].v3_index].x - vertex[facedata[i].v1_index].x;
		AC.y = vertex[facedata[i].v3_index].y - vertex[facedata[i].v1_index].y;
		AC.z = vertex[facedata[i].v3_index].z - vertex[facedata[i].v1_index].z;
		//N = AB * AC
		norm_result = crossProduct(AB, AC);
		//normalize the result(calculation the result vector length)
		float l = sqrt(norm_result.x * norm_result.x + norm_result.y * norm_result.y + norm_result.z * norm_result.z);
		//save the normal vector
		facedata[i].norm.x = norm_result.x / l;
		facedata[i].norm.y = norm_result.y / l;
		facedata[i].norm.z = norm_result.z / l;
	}
}

vector_3D objLoader::crossProduct(vector_3D A, vector_3D B){
	vector_3D result;
	result.x = A.y * B.z - A.z * B.y;
	result.y = A.z * B.x - A.x * B.z;
	result.z = A.x * B.y - A.y * B.x;
	return result;
}