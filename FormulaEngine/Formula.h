#pragma once



enum ResultCode {
	RESULT_CODE_OK,
	RESULT_CODE_SYNTAX_ERROR,
	RESULT_CODE_MISSING_DEFINITION,
	RESULT_CODE_TYPE_ERROR,
};

enum ResultType {
	RESULT_TYPE_TOKEN,
	RESULT_TYPE_SCALAR,
	RESULT_TYPE_VECTOR2,
};

struct Result {
	ResultCode code;
	ResultType type   = RESULT_TYPE_SCALAR;
	double     value  = 0.0;
	double     value2 = 0.0;
	unsigned   token  = 0;
	unsigned   scope  = 0;
};

struct ListResult {
	ResultCode code;
	std::vector<double> values;
};


struct IFormulaContext {
	virtual ~IFormulaContext() { }

	virtual Result ResolveNumber(const IFormulaContext & context, unsigned scope, unsigned token) const = 0;
	virtual ListResult ResolveList(const IFormulaContext & context, unsigned scope, unsigned token) const = 0;

	virtual bool ResolveToken (unsigned scope, unsigned token, std::string * out) const {
		ref(scope);
		ref(token);
		ref(out);
		return false;
	}

	virtual const IFormulaContext * ResolveContext (unsigned scope) const {
		ref(scope);
		return const_cast<IFormulaContext *>(this);
	}
};


typedef Result (*FTerminalEvaluator)(const IFormulaContext * context, const class Formula & termSource, unsigned * pindex);




class Formula {
public:			// Construction
	Formula();

	Formula (Formula && other);
	Formula (const Formula & other);

public:			// Assignment
	Formula & operator= (const Formula & other);

public:			// Enumerations
	enum Operator {
		OPERATOR_ADD,
		OPERATOR_SUBTRACT,
		OPERATOR_MULTIPLY,
		OPERATOR_DIVIDE,
		OPERATOR_ERROR
	};

public:			// Setup interface
	void Push (double literalValue);
	void Push (FTerminalEvaluator evaluator);
	void Push (Operator op);
	void Push (unsigned scope, unsigned token);

public:			// Evaluation interface
	Result Evaluate (const IFormulaContext * context) const;
	Result EvaluateSubexpression (const IFormulaContext * context, unsigned * pindex) const;
	bool EvaluateScopedToken (unsigned index, unsigned * outScope, unsigned * outToken) const;

private:		// Internal helpers
	Result EvaluateFunction (const IFormulaContext * context, unsigned * pindex) const;
	Result EvaluateTerminal (const IFormulaContext * context, unsigned index) const;

private:		// Internal helper structures
	struct Term {
		enum Type {
			TERM_TYPE_LITERAL,
			TERM_TYPE_EVALUATOR,
			TERM_TYPE_OPERATOR,
			TERM_TYPE_TOKEN,
		};

		union PayloadUnion {
			double				 literalValue;
			FTerminalEvaluator   evaluator;
			Operator			 op;

			struct ScopedToken {
				unsigned scope;
				unsigned token;
			} scopedToken;
		} payload;

		Type type;
	};

private:		// Internal state
	Term m_termBuffer[20];			// TODO - growth support?
	unsigned m_termCount;

private:		// Shared state
	static unsigned s_evaluationCounter;
public:
	static unsigned GetEvaluationCounter ();
};


