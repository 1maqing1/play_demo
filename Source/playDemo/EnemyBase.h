#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyDeathDelegate);

UCLASS()
class MULTIPLAYERDEMO_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	// ��������������пͻ����Զ�ͬ��
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

	// ������ֵ�仯ʱ�����пͻ��˻�����������
	UFUNCTION()
	void OnRep_CurrentHealth();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ============ 新增死亡和得分相关属性 ============
	
	/** 死亡后延迟销毁的时间 */
	UPROPERTY(EditAnywhere, Category = "Death")
	float DeferredDestructionTime = 5.0f;

	/** 团队ID，用于得分统计 */
	UPROPERTY(EditAnywhere, Category = "Team")
	uint8 TeamByte = 1;

	/** 是否已经死亡 */
	bool bIsDead = false;

	/** 死亡计时器 */
	FTimerHandle DeathTimer;

	/** 死亡时广播的委托 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEnemyDeathDelegate OnEnemyDeath;

	/** 处理死亡逻辑 */
	void Die();

	/** 延迟销毁 */
	void DeferredDestruction();

public:
	// �����������������������˺�
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float DamageAmount);

	// ��ͼ���Զ�ȡ����ֵ�ٷֱ�
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	// �Ƿ񻹻���
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const;
};