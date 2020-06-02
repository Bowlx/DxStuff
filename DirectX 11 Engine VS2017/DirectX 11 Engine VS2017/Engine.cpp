#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	timer.Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!gfx.Initialize(this->render_window.GetHWND(), width, height))
		return false;

	return true;
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update()
{
	float dt = timer.GetMilisecondsElapsed();
	timer.Restart();

	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.camera.AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0);
				//this->gfx.light.AdjustRotation(0, (float)me.GetPosX() * 0.01f, 0);
				//this->gfx.modelPlayer.AdjustRotation(0, (float)me.GetPosX() * 0.01f, 0);
				for (auto i = 0; i < this->gfx.models.size(); i++)
				{
					/*if (this->gfx.modelPlayer.GetBoundingSphere().Intersects(this->gfx.models[i].GetBoundingSphere()) || this->gfx.models[i].bounded)
					{
						if (this->gfx.modelPlayer.getScale() < this->gfx.models[i].getScale()) return;
						if (!this->gfx.models[i].bounded) {
							this->gfx.models[i].SetBounded(true);
							this->gfx.catsIn++;
							this->gfx.modelPlayer.AdjustScale(0.02);
			
							
						}
						this->gfx.models[i].SetPosition(this->gfx.modelPlayer.GetPositionVector());
						this->gfx.models[i].UpdateRotationMatrix(XMMatrixTranslationFromVector((this->gfx.modelPlayer.GetForwardVector()) * this->gfx.modelPlayer.GetBoundingSphere().Radius * 1.5)
							*
							XMMatrixRotationAxis(this->gfx.modelPlayer.GetForwardVector(), this->gfx.models[i].getRotatorX())

							*
							XMMatrixRotationAxis(this->gfx.modelPlayer.GetRightVector(), this->gfx.models[i].getRotatorZ())

						);
						this->gfx.models[i].AdjustRotation(0, (float)me.GetPosX() * 0.01f, 0);
					}*/
				}
				
			}
		}
	}

	

	const float cameraSpeed = 0.006f;
	if (keyboard.KeyIsPressed('W'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed * dt);
		this->gfx.modelPlayer.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed * dt);
		for (auto i = 0; i < this->gfx.models.size(); i++)
		{
			/*if (this->gfx.modelPlayer.GetBoundingSphere().Intersects(this->gfx.models[i].GetBoundingSphere()) || this->gfx.models[i].bounded)
			{
				if (this->gfx.modelPlayer.getScale() < this->gfx.models[i].getScale()) return;
				if (!this->gfx.models[i].bounded) {
					this->gfx.models[i].SetBounded(true); this->gfx.catsIn++;
					this->gfx.modelPlayer.AdjustScale(0.02);
					
				}
				
				this->gfx.models[i].UpdateRotationMatrix(XMMatrixTranslationFromVector((this->gfx.modelPlayer.GetForwardVector()) * this->gfx.modelPlayer.GetBoundingSphere().Radius * 1.5)
					*
					XMMatrixRotationAxis(this->gfx.modelPlayer.GetForwardVector(), this->gfx.models[i].getRotatorX())

					*
					XMMatrixRotationAxis(this->gfx.modelPlayer.GetRightVector(), this->gfx.models[i].updateRotatorZ(0.006f* dt))

				);
				this->gfx.models[i].SetPosition(this->gfx.modelPlayer.GetPositionVector());
			}*/
		}
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed * dt);
		this->gfx.modelPlayer.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed * dt);

		for (auto i = 0; i < this->gfx.models.size(); i++)
		{
			/*if (this->gfx.modelPlayer.GetBoundingSphere().Intersects(this->gfx.models[i].GetBoundingSphere()) || this->gfx.models[i].bounded)
			{
				if (this->gfx.modelPlayer.getScale() < this->gfx.models[i].getScale()) return;
				if (!this->gfx.models[i].bounded) {
					this->gfx.models[i].SetBounded(true); this->gfx.catsIn++;
					this->gfx.modelPlayer.AdjustScale(0.02);
		
				}
				this->gfx.models[i].SetPosition(this->gfx.modelPlayer.GetPositionVector());
				this->gfx.models[i].UpdateRotationMatrix(XMMatrixTranslationFromVector((this->gfx.modelPlayer.GetForwardVector() ) * this->gfx.modelPlayer.GetBoundingSphere().Radius * 1.5)
					*
					XMMatrixRotationAxis(this->gfx.modelPlayer.GetForwardVector(), this->gfx.models[i].getRotatorX())
					*
					XMMatrixRotationAxis(this->gfx.modelPlayer.GetRightVector(), this->gfx.models[i].updateRotatorZ(-0.006f * dt))
					
					
					);
			}*/
		}
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed * dt);
		this->gfx.modelPlayer.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed * dt);
		for (auto i = 0; i < this->gfx.models.size(); i++)
		{
			/*if (this->gfx.modelPlayer.GetBoundingSphere().Intersects(this->gfx.models[i].GetBoundingSphere()) || this->gfx.models[i].bounded)
			{
				if (this->gfx.modelPlayer.getScale() < this->gfx.models[i].getScale()) return;
				if (!this->gfx.models[i].bounded) {
					this->gfx.models[i].SetBounded(true); this->gfx.catsIn++;
					this->gfx.modelPlayer.AdjustScale(0.02);

				}
				this->gfx.models[i].SetPosition(this->gfx.modelPlayer.GetPositionVector());
		
				this->gfx.models[i].UpdateRotationMatrix(XMMatrixTranslationFromVector((this->gfx.modelPlayer.GetForwardVector() ) * this->gfx.modelPlayer.GetBoundingSphere().Radius*1.5)
					* 
					XMMatrixRotationAxis(this->gfx.modelPlayer.GetRightVector(), this->gfx.models[i].getRotatorZ())
					*
					XMMatrixRotationAxis(this->gfx.modelPlayer.GetForwardVector(), this->gfx.models[i].updateRotatorX(0.006f * dt)));
			}*/
		}
		
		
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed * dt);
		this->gfx.modelPlayer.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed * dt);

		for (auto i = 0; i < this->gfx.models.size(); i++)
		{
			/*if (this->gfx.modelPlayer.GetBoundingSphere().Intersects(this->gfx.models[i].GetBoundingSphere()) || this->gfx.models[i].bounded)
			{
				if (this->gfx.modelPlayer.getScale() < this->gfx.models[i].getScale()) return;
				if (!this->gfx.models[i].bounded) {
					this->gfx.models[i].SetBounded(true); this->gfx.catsIn++;
					this->gfx.modelPlayer.AdjustScale(0.02);
	
				}
				this->gfx.models[i].SetPosition(this->gfx.modelPlayer.GetPositionVector());
				this->gfx.models[i].UpdateRotationMatrix(XMMatrixTranslationFromVector((this->gfx.modelPlayer.GetForwardVector() )* this->gfx.modelPlayer.GetBoundingSphere().Radius*1.5)
					* 
					XMMatrixRotationAxis(this->gfx.modelPlayer.GetRightVector(), this->gfx.models[i].getRotatorZ())
					*
					XMMatrixRotationAxis(this->gfx.modelPlayer.GetForwardVector(), this->gfx.models[i].updateRotatorX(-0.006f * dt)));
			}*/
		}

		
	}
	if (keyboard.KeyIsPressed(VK_SPACE))
	{
		this->gfx.camera.AdjustPosition(0.0f, cameraSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('Z'))
	{
		this->gfx.camera.AdjustPosition(0.0f, -cameraSpeed * dt, 0.0f);
	}

}

void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}

