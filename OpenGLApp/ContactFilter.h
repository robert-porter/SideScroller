#pragma once

class Body;
class Contact;

class ContactFilter
{
public:
	virtual			~ContactFilter() {}
	virtual bool	ShouldCollide(const Body *b1, const Body *b2) { }
};

/*
	Do not add or remove bodies during any of these callbacks
	TODO: assert if someone tries to add or remove bodies during callbacks
	(lock world??)
*/
class ContactListener
{
public:
	virtual			~ContactListener() {}
	virtual void	OnSensor(Fixture *a, Fixture *b) { }
	virtual void	OnContact(Contact* contact) { }
	virtual void	OnSeperation(Contact *contact) {}
	virtual void	PreSolve(Contact *contact) {}
	virtual void	PostSolve(Contact *contact) {}
};
