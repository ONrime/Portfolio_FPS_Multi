// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/CameraShake/Public/Player_Splint_CameraShake.h"

UPlayer_Splint_CameraShake::UPlayer_Splint_CameraShake()
{
    bSingleInstance = true; // ���� �ϳ��� ���� ��Ű��

    OscillationDuration = -0.2f;
    OscillationBlendInTime = 0.3f;
    OscillationBlendOutTime = 0.3f;

    RotOscillation.Pitch.Amplitude = 0.5f; //���� (����)
    RotOscillation.Pitch.Frequency = 15.0f; //������ (�ӵ�)
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom; //�ʱ� �ɼ�
    RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave;
}