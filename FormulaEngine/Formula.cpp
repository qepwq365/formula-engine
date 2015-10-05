#include "Pch.h"

#include "Formula.h"



Formula::Formula() {
	m_terms.reserve(2);
}


Formula::Formula(Formula && other) {
	std::swap(m_terms, other.m_terms);
}

Formula & Formula::operator = (Formula && other) {
	std::swap(m_terms, other.m_terms);
	return *this;
}


Result Formula::Evaluate(const IFormulaContext * context) const {
	Result ret;

	ret.code = RESULT_CODE_OK;
	if(m_terms.empty()) {
		ret.value = 0.0;
		return ret;
	}

	unsigned index = static_cast<unsigned>(m_terms.size()) - 1;
	ret = EvaluateSubexpression(context, &index);

	if(index > 0) {
		ret.code = RESULT_CODE_SYNTAX_ERROR;
		ret.value = 0.0;
	}

	return ret;
}


Result Formula::EvaluateFunction(const IFormulaContext * context, unsigned * pindex) const {
	return m_terms[*pindex].payload.evaluator->Evaluate(context, *this, pindex);
}

Result Formula::EvaluateSubexpression(const IFormulaContext * context, unsigned * pindex) const {
	if(m_terms[*pindex].flags & Term::FLAG_IS_EVALUATOR)
		return EvaluateFunction(context, pindex);

	if((*pindex == 0) || !(m_terms[*pindex].flags & Term::FLAG_IS_OPERATOR))
		return EvaluateTerminal(context, *pindex);

	Operator op = m_terms[*pindex].payload.op;

	--(*pindex);
	Result right = EvaluateSubexpression(context, pindex);
	if(right.code != RESULT_CODE_OK)
		return right;

	--(*pindex);
	Result left = EvaluateSubexpression(context, pindex);
	if(left.code != RESULT_CODE_OK)
		return left;

	// TODO - allow mismatched type for scaling vectors via mul/div?
	if(left.type != right.type) {
		Result err;
		err.code = RESULT_CODE_TYPE_ERROR;
		return err;
	}

	Result ret;
	ret.code = RESULT_CODE_OK;
	ret.type = left.type;

	switch(op) {
	case OPERATOR_ADD:			ret.value = left.value + right.value;		break;
	case OPERATOR_SUBTRACT:		ret.value = left.value - right.value;		break;
	case OPERATOR_MULTIPLY:		ret.value = left.value * right.value;		break;
	case OPERATOR_DIVIDE:		ret.value = left.value / right.value;		break;
	default:
		ret.code = RESULT_CODE_SYNTAX_ERROR;
		ret.value = 0.0;
		return ret;
	}

	if(left.type == RESULT_TYPE_VECTOR2) {
		switch(op) {
		case OPERATOR_ADD:			ret.value2 = left.value2 + right.value2;		break;
		case OPERATOR_SUBTRACT:		ret.value2 = left.value2 - right.value2;		break;
		case OPERATOR_MULTIPLY:		ret.value2 = left.value2 * right.value2;		break;	// TODO - what does it mean to mul/div a vec2?
		case OPERATOR_DIVIDE:		ret.value2 = left.value2 / right.value2;		break;
		}
	}

	return ret;
}

Result Formula::EvaluateTerminal(const IFormulaContext * context, unsigned index) const {
	Result ret;

	if(m_terms[index].flags & Term::FLAG_IS_TOKEN) {
		ret = context->ResolveNumber(*context, m_terms[index].payload.scopedToken.scope, m_terms[index].payload.scopedToken.token);
		return ret;
	}

	if(!(m_terms[index].flags & Term::FLAG_IS_LITERAL)) {
		ret.code = RESULT_CODE_SYNTAX_ERROR;
		ret.value = 0.0;
		return ret;
	}

	ret.code = RESULT_CODE_OK;
	ret.value = m_terms[index].payload.literalValue;
	return ret;
}


void Formula::Push(double literalValue) {
	Term t;
	t.flags = Term::FLAG_IS_LITERAL;
	t.payload.literalValue = literalValue;

	m_terms.emplace_back(t);
}

void Formula::Push(const ITerminalEvaluator & evaluator) {
	Term t;
	t.flags = Term::FLAG_IS_EVALUATOR;
	t.payload.evaluator = const_cast<ITerminalEvaluator *>(&evaluator);

	m_terms.emplace_back(t);
}

void Formula::Push(Operator op) {
	Term t;
	t.flags = Term::FLAG_IS_OPERATOR;
	t.payload.op = op;

	m_terms.emplace_back(t);
}

void Formula::Push(unsigned scope, unsigned token) {
	Term t;
	t.flags = Term::FLAG_IS_TOKEN;
	t.payload.scopedToken.scope = scope;
	t.payload.scopedToken.token = token;

	m_terms.emplace_back(t);
}

