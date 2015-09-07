#pragma once


struct IActionPerformer;


class Scriptable {
public:			// Construction and destruction
	Scriptable() { }
	Scriptable(Scriptable && other);
	~Scriptable();

private:		// Non-copyable
	Scriptable(const Scriptable & other) = delete;
	Scriptable & operator = (const Scriptable & other) = delete;

public:			// Component accessors
	const ScopedPropertyBag & GetScopes() const		{ return m_scopes; }
	ScopedPropertyBag & GetScopes()					{ return m_scopes; }

	EventHandlerSet & GetEvents()					{ return m_eventHandlers; }

public:			// Membership notification interface
	void OnListMembershipAdded(unsigned listToken, IActionPerformer * owner) const;
	void OnListMembershipRemoved(unsigned listToken, IActionPerformer * owner) const;

private:		// Internal helper structures
	struct Membership {
		IActionPerformer * owner;
		unsigned token;
	};

private:		// Internal state
	ScopedPropertyBag m_scopes;
	EventHandlerSet   m_eventHandlers;

	mutable std::vector<Membership> m_listMemberships;
};


