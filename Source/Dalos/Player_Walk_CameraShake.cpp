// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Walk_CameraShake.h"

UPlayer_Walk_CameraShake::UPlayer_Walk_CameraShake()
{
    bSingleInstance = true; // ���� �ϳ��� ���� ��Ű��

    OscillationDuration = -0.2f;
    OscillationBlendInTime = 0.3f;
    OscillationBlendOutTime = 0.3f;

    RotOscillation.Pitch.Amplitude = 0.3f; //���� (����)
    RotOscillation.Pitch.Frequency = 10.0f; //������ (�ӵ�)
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom; //�ʱ� �ɼ�
    RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave;
}