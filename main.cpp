/**
 *
 * PuyoSimuVSClient
 * main.cpp
 * 
 * @author REF
 */
#include "DxLib.h"
#include "input.h"
#include "Fps.h"
#include "SceneGame.h"
/*----------------------------------------------------------------------*/
//      ���[�v�ŕK���s������                             
/*----------------------------------------------------------------------*/
int ProcessLoop(){
    if(ProcessMessage()!=0)return -1;//�v���Z�X�������G���[�Ȃ�-1��Ԃ�
    if(ClearDrawScreen()!=0)return -1;//��ʃN���A�������G���[�Ȃ�-1��Ԃ�
    UpdateInputInfo();//���݂�input���̍X�V���s��
    return 0;
}
/*----------------------------------------------------------------------*/
//      Fps�����܂�ScreenFlip                             
/*----------------------------------------------------------------------*/
int ScreenFlip_2(){
	static Fps fps;

	fps.Update();
	DrawFormatString(0, 0, GetColor(255,255,255), "%.1f", fps.GetFps() );
	ScreenFlip();
	fps.Wait();
    return 0;
}

/*----------------------------------------------------------------------*/
//      main
/*----------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	SetDoubleStartValidFlag( TRUE );           // ���d�N��������
	SetAlwaysRunFlag( TRUE );                  // ��A�N�e�B�u�ł�����������
	ChangeWindowMode(TRUE);                    // �E�B���h�E���[�h
	SetOutApplicationLogValidFlag( FALSE );    // ���O�̏o�͗}��
	SetMainWindowText( "PuyoSimuVSClient" ) ;  // �E�B���h�E�^�C�g��
	SetWindowUserCloseEnableFlag(FALSE);       // �E�B���h�E�o�c�{�^���ŋ����I�����Ȃ�
    if(DxLib_Init() == -1 || SetDrawScreen( DX_SCREEN_BACK )!=0) return -1; //�������Ɨ���ʉ�
	//SetDragFileValidFlag(TRUE);                // D&D������
	
	//�f�o�b�O�p�ɃR���\�[�����Ăяo��
	FILE *dbgstream;
	{
		// TODO:�������͍� ����AllocConsole()�̑O��cout�Ƃ��o�͂����Ⴄ�ƃo�O��
        AllocConsole();
        freopen_s( &dbgstream, "CONOUT$", "w", stdout);
        //freopen("debug.txt", "w", stdout); 
        //freopen("CONIN$", "r", stdin);
	}

	SceneGameInit();
	while( ProcessLoop()==0 && !GetWindowUserCloseFlag() ){ //���C�����[�v
		SceneGameUpdate();
		SceneGameDraw();
		ScreenFlip_2(); //����ʂ𔽉f���đҋ@
	}
	SceneGameFin();

	fclose( dbgstream );
	FreeConsole();
    DxLib_End();
    return 0;
}

