#pragma once
#include "Mesh.h"
#include <DirectXCollision.h>

using namespace DirectX;

class Model
{
public:
	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * texture, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);
	void SetTexture(ID3D11ShaderResourceView * texture);
	void Draw(const XMMATRIX & viewProjectionMatrix);
	bool bounded;

	
	const XMVECTOR & GetPositionVector() const;
	const XMFLOAT3 & GetPositionFloat3() const;
	const XMVECTOR & GetRotationVector() const;
	const XMFLOAT3 & GetRotationFloat3() const;
	
	void SetBounded(bool);
	void SetPosition(const XMVECTOR & pos);
	void SetPosition(const XMFLOAT3 & pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR & pos);
	void AdjustPosition(const XMFLOAT3 & pos);
	void AdjustPosition(float x, float y, float z);
	void AdjustPositionNoUpdate(const XMVECTOR& pos);
	void AdjustPositionNoUpdate(const XMFLOAT3& pos);
	void AdjustPositionNoUpdate(float x, float y, float z);
	void SetRotation(const XMVECTOR & rot);
	void SetRotation(const XMFLOAT3 & rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const XMVECTOR & rot);
	void AdjustRotation(const XMFLOAT3 & rot);
	void AdjustRotation(float x, float y, float z);
	void SetScale(const XMVECTOR& rot);
	void SetScale(const XMFLOAT3& rot);
	void SetScale(float x, float y, float z);
	void AdjustScale(float);
	void SetBoundingSphereCenter (XMVECTOR& center);
	void SetBoundingSphereCenter(const XMFLOAT3& center);
	void SetBoundingSphereCenter(float x, float y, float z);
	void SetBoundingSphereRadius(float r);
	void UpdateRotationMatrix(XMMATRIX);
	void SetLookAtPos(XMFLOAT3 lookAtPos);
	const XMVECTOR & GetForwardVector();
	const XMVECTOR & GetRightVector();
	const XMVECTOR & GetBackwardVector();
	const XMVECTOR & GetLeftVector();
	const XMMATRIX &  getWorldMatrix();
	const XMMATRIX& getRotationMatrix();
	const BoundingSphere& GetBoundingSphere();
	float updateRotatorZ(float);
	float getRotatorZ();
	float updateRotatorX(float);
	float getRotatorX();
	float getScale();
private:
	std::vector<Mesh> meshes;
	bool LoadModel(const std::string & filePath);
	void ProcessNode(aiNode * node, const aiScene * scene);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene);
	void UpdateWorldMatrix();


	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader = nullptr;
	ID3D11ShaderResourceView * texture = nullptr;

	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;

	float rotatorZ;
	float rotatorX;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
	BoundingSphere bounding_sphere;
};


