#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class MULTIPLAYERDEMO_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	// 这个变量会在所有客户端自动同步
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

	// 当生命值变化时，所有客户端会调用这个函数
	UFUNCTION()
	void OnRep_CurrentHealth();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 服务器调用这个函数来造成伤害
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float DamageAmount);

	// 蓝图可以读取生命值百分比
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	// 是否还活着
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const;
};