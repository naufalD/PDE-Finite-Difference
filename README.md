## Introduction
The finite difference method is used to solve partial differential equations. In this case, we consider the finding the electric potential between charged plates by numerically solving Laplace's equation. This is part of exercise 17.4 in the computational physics textbook by Landau (2015)[^1] on PDEs. This project makes use of Metal shaders for visualization.

## Theory
In classical electrodynamics, the electric potential due to a static charge distribution satifies the Poisson's equation in the form of <br/>

$$\nabla^{2}U(\mathbf{x})=-4\pi\rho(\mathbf{x})$$

Where $U(x)$ is the elctric potential and $\rho(x)$ is the charge distribution. In 2-dimensions this is then <br/>

$$\frac{\partial^{2}U(x,y)}{\partial x^2}+ \frac{\partial^{2}U(x,y)}{\partial y^2}=-4\pi\rho(x,y)$$

Following the derivation for the forward difference method for numerical differentiation we get<br/>

$$U(x+\triangle,y)+U(x-\triangle,y)+U(x,y+\triangle)+U(x,y-\triangle)-4U(x,y)=-4\pi\rho(x,y)\triangle^{2}$$

From this, we get the equation for the finite difference method for Poisson's equation as<br/>

$$U(x,y)=\frac{1}{4}(U(x+\triangle,y)+U(x-\triangle,y)+U(x,y+\triangle)+U(x,y-\triangle))+\pi \rho(x,y)\triangle^{2}$$


## Program Example
In the below example, we solve the electric potential between 2 rings with Laplace's equation by setting the charge density as 0 everywhere. By using a plane with discrete values we get <br/>

$$U(x,y)=\frac{1}{4}(U(x+1,y)+U(x-1,y)+U(x,y+1)+U(x,y-1))$$

![img](media/Charged%20ring.png)


[^1]: Landau, R. H. (2015). Computational physics problem solving with python. John Wiley & Sons, Incorporated. 