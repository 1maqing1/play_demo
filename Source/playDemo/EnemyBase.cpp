#include "EnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

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
}

void AEnemyBase::ApplyDamage(float DamageAmount)
{
	// 只在服务器上执行伤害逻辑
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (!IsAlive())
		return;

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	if (!IsAlive())
	{
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