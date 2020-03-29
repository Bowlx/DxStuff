#include "Model.h"
#include <iostream>

bool Model::Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->texture = texture;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;
	bounded = false;
	rotatorZ = 0;
	rotatorX = 0;

	try
	{
		if (!this->LoadModel(filePath))
			return false;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->SetScale(1, 1, 1);
	this->UpdateWorldMatrix();
	return true;
}



void Model::SetTexture(ID3D11ShaderResourceView * texture)
{
	this->texture = texture;
}

void Model::Draw(const XMMATRIX & viewProjectionMatrix)
{
	//Update Constant buffer with WVP Matrix
	this->cb_vs_vertexshader->data.WVP = this->worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
	this->cb_vs_vertexshader->data.WVP = XMMatrixTranspose(this->cb_vs_vertexshader->data.WVP);
	this->cb_vs_vertexshader->data.World = XMMatrixTranspose(this->worldMatrix);
	this->cb_vs_vertexshader->ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());

	this->deviceContext->PSSetShaderResources(0, 1, &this->texture); //Set Texture

	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw();
	}
}

void Model::DrawToDepth(const XMMATRIX& viewProjectionMatrix)
{
	this->cb_vs_vertexshader->data.WVP = this->worldMatrix* viewProjectionMatrix; //Calculate World-View-Projection Matrix
	this->cb_vs_vertexshader->data.WVP = XMMatrixTranspose(this->cb_vs_vertexshader->data.WVP);
	this->cb_vs_vertexshader->data.World = XMMatrixTranspose(this->worldMatrix);
	this->cb_vs_vertexshader->ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());

	this->deviceContext->PSSetShaderResources(0, 1, &this->texture); //Set Texture

	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw();
	}
}




bool Model::LoadModel(const std::string & filePath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
												aiProcess_Triangulate |
												aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->ProcessNode(pScene->mRootNode, pScene);
	return true;
}

void Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;
		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}
	

		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh(this->device, this->deviceContext, vertices, indices);
}

void Model::UpdateWorldMatrix()
{
	this->worldMatrix = XMMatrixScaling(this->scale.x,this->scale.y,this->scale.z)* XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * rotationMatrix *XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z) ;
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x,this->rot.y, this->rot.z);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}

const XMVECTOR & Model::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3 & Model::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR & Model::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3 & Model::GetRotationFloat3() const
{
	return this->rot;
}

void Model::SetBounded(bool status)
{
	this->bounded = status;
}

void Model::SetPosition(const XMVECTOR & pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	SetBoundingSphereCenter(this->posVector);
	this->UpdateWorldMatrix();
}

void Model::SetPosition(const XMFLOAT3 & pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	SetBoundingSphereCenter(this->posVector);
	this->UpdateWorldMatrix();
}

void Model::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	SetBoundingSphereCenter(this->posVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	SetBoundingSphereCenter(this->posVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMFLOAT3 & pos)
{
	this->pos.x += pos.y;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	SetBoundingSphereCenter(this->posVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	SetBoundingSphereCenter(this->posVector);
	this->UpdateWorldMatrix();
}
void Model::AdjustPositionNoUpdate(const XMVECTOR& pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	SetBoundingSphereCenter(this->posVector);
	//this->UpdateWorldMatrix();
}

void Model::AdjustPositionNoUpdate(const XMFLOAT3& pos)
{
	this->pos.x += pos.y;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	SetBoundingSphereCenter(this->posVector);
	//this->UpdateWorldMatrix();
}

void Model::AdjustPositionNoUpdate(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	SetBoundingSphereCenter(this->posVector);
	//this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMFLOAT3 & rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMFLOAT3 & rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::SetScale(const XMVECTOR& scale)
{
	XMStoreFloat3(&this->scale, scale);
	SetBoundingSphereRadius(this->scale.x*2);
	this->UpdateWorldMatrix();
	
}

void Model::SetScale(const XMFLOAT3& scale)
{
	this->scale = scale;
	SetBoundingSphereRadius(this->scale.x*2);
	this->UpdateWorldMatrix();
}

void Model::SetScale(float x, float y, float z)
{
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;
	SetBoundingSphereRadius(this->scale.x*2);
	this->UpdateWorldMatrix();
}

void Model::AdjustScale(float val)
{
	this->scale.x += val;
	this->scale.y += val;
	this->scale.z += val;
	SetBoundingSphereRadius(this->scale.x * 2);
	this->UpdateWorldMatrix();
}

void Model::SetBoundingSphereCenter(XMVECTOR& center)
{
	XMStoreFloat3(&this->bounding_sphere.Center, center);
}

void Model::SetBoundingSphereCenter(const XMFLOAT3& center)
{
	this->bounding_sphere.Center = center;
}

void Model::SetBoundingSphereCenter(float x, float y, float z)
{
	this->bounding_sphere.Center.x = x;
	this->bounding_sphere.Center.y = y;
	this->bounding_sphere.Center.z = z;
}

void Model::SetBoundingSphereRadius(float r)
{
	this->bounding_sphere.Radius = r;
}
void Model::UpdateRotationMatrix(XMMATRIX rotmat)
{
	this->rotationMatrix = rotmat;
}


void Model::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR & Model::GetForwardVector()
{
	return this->vec_forward;
}

const XMVECTOR & Model::GetRightVector()
{
	return this->vec_right;
}

const XMVECTOR & Model::GetBackwardVector()
{
	return this->vec_backward;
}

const XMVECTOR & Model::GetLeftVector()
{
	return this->vec_left;
}

const XMMATRIX& Model::getWorldMatrix()
{
	return this->worldMatrix;
}

const XMMATRIX& Model::getRotationMatrix()
{
	return this->rotationMatrix;
}

const BoundingSphere& Model::GetBoundingSphere()
{
	return this->bounding_sphere;
}

float Model::updateRotatorZ(float up)
{
	rotatorZ += up;
	if (std::abs(rotatorZ) >= 6.26f) rotatorZ = 0.0;
	return rotatorZ;
}

float Model::getRotatorZ()
{
	return  rotatorZ;
}
float Model::updateRotatorX(float up)
{
	rotatorX += up;
	if (std::abs(rotatorX) >= 6.26f) rotatorX = 0.0;
	return rotatorX;
}

float Model::getRotatorX()
{
	return  rotatorX;
}

float Model::getScale()
{
	return  this->scale.x;
}
