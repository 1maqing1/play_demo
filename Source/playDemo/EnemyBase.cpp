#include "EnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"  // 添加这一行，用于屏幕打印

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 重要：让这个角色在网络中复制
	bReplicates = true;

	// 移动也要同步
	SetReplicateMovement(true);
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 告诉UE：CurrentHealth 需要在所有客户端同步
	DOREPLIFETIME(AEnemyBase, CurrentHealth);
}

void AEnemyBase::OnRep_CurrentHealth()
{
	// 当生命值更新时，这里可以触发蓝图中播放受伤动画等
	// 蓝图中可以绑定这个事件
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("OnRep_CurrentHealth 被调用"));
}

void AEnemyBase::ApplyDamage(float DamageAmount)
{
	// 只在服务器上执行伤害逻辑
	if (GetLocalRole() != ROLE_Authority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ApplyDamage: 不是服务器，跳过"));
		return;
	}

	if (!IsAlive())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ApplyDamage: 敌人已死，跳过"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("敌人受到伤害: %.0f, 当前生命: %.0f"), DamageAmount, CurrentHealth));

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	if (!IsAlive())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("敌人死亡！"));
		// 死亡：可以销毁，或者播放死亡动画后销毁
		Destroy();
	}
}

float AEnemyBase::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

bool AEnemyBase::IsAlive() const
{
	return CurrentHealth > 0;
}