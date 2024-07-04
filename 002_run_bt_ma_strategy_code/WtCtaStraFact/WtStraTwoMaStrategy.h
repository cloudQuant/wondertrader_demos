#pragma once
#include "../Includes/CtaStrategyDefs.h"

class WtStraTwoMaStrategy : public CtaStrategy
{
public:
	WtStraTwoMaStrategy(const char* id);
	virtual ~WtStraTwoMaStrategy();

public:
	virtual const char* getFactName() override;

	virtual const char* getName() override;

	virtual bool init(WTSVariant* cfg) override;

	virtual void on_schedule(ICtaStraCtx* ctx, uint32_t curDate, uint32_t curTime) override;

	virtual void on_init(ICtaStraCtx* ctx) override;

	virtual void on_tick(ICtaStraCtx* ctx, const char* stdCode, WTSTickData* newTick) override;

private:
	//ָ�����
	uint32_t	short_days;
	uint32_t    long_days;

	//��������
	std::string _period;
	//K������
	uint32_t	_count;

	//��Լ����
	std::string _code;

	bool		_isstk;
};

