from pathlib import Path
from typing import List, Optional
from sqlalchemy import ForeignKey, String, Integer, Float
from sqlalchemy.orm import DeclarativeBase, Mapped, mapped_column, relationship

# Possibly add category


class Base(DeclarativeBase):
    pass


class User(Base):
    __tablename__ = "users"
    id: Mapped[int] = mapped_column(primary_key=True)
    name: Mapped[str] = mapped_column(String(30))
    surname: Mapped[str] = mapped_column(String(50))
    email: Mapped[str] = mapped_column(String(50))
    phone: Mapped[Optional[str]] = mapped_column(String(9))
    balance: Mapped[float] = mapped_column(Float, default=0)
    products_sold_: Mapped[List["Product"]] = relationship(
        back_populates="user", cascade="all, delete-orphan"
    )
    products_bought_: Mapped[List["ProductBought"]] = relationship(
        back_populates="user", cascade="all, delete-orphan"
    )

class ProductBought(Base):
    __tablename__ = "products_bought"
    id: Mapped[int] = mapped_column(primary_key=True)
    buyer_id: Mapped[int] = mapped_column(ForeignKey("users.id"))
    product_id: Mapped[int] = mapped_column(ForeignKey("products.id"))
    amount: Mapped[int] = mapped_column(Integer())
    user: Mapped["User"] = relationship(back_populates="products_bought_")
    price: Mapped[float] = mapped_column(Float, default=0) # Price can change

class Category(Base):
    __tablename__ = "categories"
    id: Mapped[int] = mapped_column(primary_key=True)
    name: Mapped[str] = mapped_column(String(50))

class Product(Base):
    __tablename__ = "products"
    id: Mapped[int] = mapped_column(primary_key=True)
    title: Mapped[str] = mapped_column(String(100))
    description: Mapped[str] = mapped_column(String(1500))
    amount: Mapped[int] = mapped_column(Integer())
    price: Mapped[float] = mapped_column(Float, default=0)
    seller_id: Mapped[int] = mapped_column(ForeignKey("users.id"))
    category_id: Mapped[int] = mapped_column(ForeignKey("categories.id"))
    user: Mapped["User"] = relationship(back_populates="products_sold_")

class Review(Base):
    __tablename__ = "reviews"
    id: Mapped[int] = mapped_column(primary_key=True)
    user_id: Mapped[int] = mapped_column(ForeignKey("users.id"))
    product_id: Mapped[int] = mapped_column(ForeignKey("products.id"))
    rating: Mapped[int] = mapped_column(Integer())
    content: Mapped[str] = mapped_column(String(1500))


def main():
    from sqlalchemy import create_engine
    path = str(Path('/AegrescitMedendo/database.db').absolute())
    # path = str(Path('/AegrescitMedendo/build/database.db').absolute())
    engine = create_engine(f"sqlite://{path}", echo=True)
    Base.metadata.create_all(engine)

    # Add one element
    from sqlalchemy.orm import sessionmaker
    Session = sessionmaker(bind = engine)
    session = Session()
    user = User(name = 'Paul', surname = '4rlenrey', email = '4rlenrey@gmail.com')
    session.add(user)
    session.commit()

if __name__ == "__main__":
    main()
