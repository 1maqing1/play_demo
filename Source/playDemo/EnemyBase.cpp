#include "EnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// 前向声明游戏模式类用于得分
class AShooterGameMode;

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 启用复制
	bReplicates = true;
	SetReplicateMovement(true);

	// 初始化生命值
	CurrentHealth = MaxHealth;
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEnemyBase, CurrentHealth);
}

void AEnemyBase::OnRep_CurrentHealth()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("OnRep_CurrentHealth ������"));
}

void AEnemyBase::ApplyDamage(float DamageAmount)
{
	// 只在服务端执行伤害逻辑
	if (GetLocalRole() != ROLE_Authority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ApplyDamage: ���Ƿ�����������"));
		return;
	}

	if (!IsAlive())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ApplyDamage: ��������������"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("�����ܵ��˺�: %.0f, ��ǰ����: %.0f"), DamageAmount, CurrentHealth));

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	if (!IsAlive())
	{
		// 调用死亡逻辑
		Destroy();
	}
}

void AEnemyBase::Die()
{
	// 防止重复调用
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("����������"));

	// 广播死亡委托
	OnEnemyDeath.Broadcast();

	// ============ 增加得分逻辑 ============
	if (AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->IncrementTeamScore(TeamByte);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("�����ɹ�����"));
	}

	// ============ 死亡物理效果 ============
	// 禁用胶囊碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 停止移动
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->StopActiveMovement();

	// 启用布娃娃物理
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetPhysicsBlendWeight(1.0f);

	// 延迟销毁
	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &AEnemyBase::DeferredDestruction, DeferredDestructionTime, false);
}

void AEnemyBase::DeferredDestruction()
{
	Destroy();
}

float AEnemyBase::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

bool AEnemyBase::IsAlive() const
{
	return CurrentHealth > 0 && !bIsDead;
}