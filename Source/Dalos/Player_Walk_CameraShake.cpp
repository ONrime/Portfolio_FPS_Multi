// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Walk_CameraShake.h"

UPlayer_Walk_CameraShake::UPlayer_Walk_CameraShake()
{
    bSingleInstance = true; // 진동 하나만 진행 시키기

    OscillationDuration = -0.2f;
    OscillationBlendInTime = 0.3f;
    OscillationBlendOutTime = 0.3f;

    RotOscillation.Pitch.Amplitude = 0.3f; //진폭 (강도)
    RotOscillation.Pitch.Frequency = 10.0f; //진동수 (속도)
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom; //초기 옵셋
    RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave;
}