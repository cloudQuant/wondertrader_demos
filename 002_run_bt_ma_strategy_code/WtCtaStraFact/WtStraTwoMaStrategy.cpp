#include "WtStraTwoMaStrategy.h"

#include "../Includes/ICtaStraCtx.h"

#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/WTSDataDef.hpp"
#include "../Share/decimal.h"

#include <iostream>

extern const char* FACT_NAME;

//By Wesley @ 2022.01.05
#include "../Share/fmtlib.h"

WtStraTwoMaStrategy::WtStraTwoMaStrategy(const char* id)
	: CtaStrategy(id)
{
}


WtStraTwoMaStrategy::~WtStraTwoMaStrategy()
{
}

const char* WtStraTwoMaStrategy::getFactName()
{
	return FACT_NAME;
}

const char* WtStraTwoMaStrategy::getName()
{
	return "DualThrust";
}

bool WtStraTwoMaStrategy::init(WTSVariant* cfg)
{
	if (cfg == NULL)
		return false;

	short_days = cfg->getUInt32("short_days");
	long_days = cfg->getDouble("long_days");

	_period = cfg->getCString("period");
	_count = cfg->getUInt32("count");
	_code = cfg->getCString("code");

	_isstk = cfg->getBoolean("stock");

	return true;
}

void WtStraTwoMaStrategy::on_schedule(ICtaStraCtx* ctx, uint32_t curDate, uint32_t curTime)
{
	std::string code = _code;
	if (_isstk)
		code += "-";
	WTSKlineSlice *kline = ctx->stra_get_bars(code.c_str(), _period.c_str(), _count, true);
	if(kline == NULL)
	{
		//����������һЩ��־
		std::cout << "kline is NULL" << std::endl;
		return;
	}

	if (kline->size() == 0)
	{
		std::cout << "kline is length 0" << std::endl;
		kline->release();
		return;
	}

	uint32_t trdUnit = 1;
	if (_isstk)
		trdUnit = 100;

	//std::cout << "short_days = " << short_days << std::endl;
	//std::cout << "long_days = " << long_days << std::endl;

	int32_t _short_days = static_cast<int32_t>(short_days);
    int32_t _long_days = static_cast<int32_t>(long_days);
    double pre_short_sum = 0;
    // ����ǰһ�ڶ��ھ���
    for (uint32_t i=2; i<=_short_days+1; i++){
        pre_short_sum+=kline->at(-1*i)->close;
    }
    double pre_short_ma = pre_short_sum / _short_days;
    // ���㵱�ڶ��ھ���
    double now_short_sum = pre_short_sum - kline->at(-1*(_short_days+1))->close + kline->at(-1)->close;
    double now_short_ma = now_short_sum / _short_days;
    // ����ǰһ�ڳ��ھ���
    double pre_long_sum = pre_short_sum;
    for (uint32_t j=_short_days+2; j<=_long_days+1; j++){
        pre_long_sum+=kline->at(-1*j)->close;
    }
	double pre_long_ma = pre_long_sum / _long_days;
    // ���㵱�ڵĳ��ھ���
    double now_long_sum = pre_long_sum - kline->at(-1*(_long_days+1))->close + kline->at(-1)->close;
    double now_long_ma = now_long_sum / _long_days;

	WTSCommodityInfo* commInfo = ctx->stra_get_comminfo(_code.c_str());

	double curPos = ctx->stra_get_position(_code.c_str()) / trdUnit;
	if(decimal::eq(curPos,0))
	{
		if(pre_short_ma < pre_long_ma && now_short_ma >= now_long_ma)
		{
			ctx->stra_enter_long(_code.c_str(), 2 * trdUnit, "DT_EnterLong");
			//����ͻ��
			ctx->stra_log_info(fmt::format("���{}>={},��ֽ���", now_short_ma, now_long_ma).c_str());
		}
		else if (pre_short_ma > pre_long_ma && now_short_ma <= now_long_ma)
		{
			ctx->stra_enter_short(_code.c_str(), 2 * trdUnit, "DT_EnterShort");
			//����ͻ��
			ctx->stra_log_info(fmt::format("����{}<={},�ղֽ���", now_short_ma, now_long_ma).c_str());
		}
	}
	//else if(curPos > 0)
	else if (decimal::gt(curPos, 0))
	{
		if(pre_short_ma > pre_long_ma && now_short_ma <= now_long_ma)
		{
			//��ֳ���
			ctx->stra_exit_long(_code.c_str(), 2 * trdUnit, "DT_ExitLong");
            ctx->stra_log_info(fmt::format("����{}<={},ƽ��", now_short_ma, now_long_ma).c_str());
		}
	}
	//else if(curPos < 0)
	else if (decimal::lt(curPos, 0))
	{
		if (pre_short_ma < pre_long_ma && now_short_ma >= now_long_ma)
		{
			//�ղֳ���
			ctx->stra_exit_short(_code.c_str(), 2 * trdUnit, "DT_ExitShort");
            ctx->stra_log_info(fmt::format("���{}>={},ƽ��", now_short_ma, now_long_ma).c_str());
		}
	}

	ctx->stra_save_user_data("test", "waht");

	//����ͷ�һ��Ҫ��
	kline->release();
}

void WtStraTwoMaStrategy::on_init(ICtaStraCtx* ctx)
{
	std::string code = _code;
	if (_isstk)
		code += "-";
	WTSKlineSlice *kline = ctx->stra_get_bars(code.c_str(), _period.c_str(), _count, true);
	if (kline == NULL)
	{
		//����������һЩ��־
		return;
	}

	kline->release();
}

void WtStraTwoMaStrategy::on_tick(ICtaStraCtx* ctx, const char* stdCode, WTSTickData* newTick)
{
	//û��ʲôҪ����
}
