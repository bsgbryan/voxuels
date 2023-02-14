// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

namespace Block {
	enum class Face {
		Front,
		Back,
		Left,
		Right,
		Up,
		Down
	};

	constexpr Face AllFaces[6] = {
		Face::Front,
		Face::Back,
		Face::Left,
		Face::Right,
		Face::Up,
		Face::Down
	};

	const FVector Vectors[6] = {
		FVector::ForwardVector,
		FVector::BackwardVector,
		FVector::LeftVector,
		FVector::RightVector,
		FVector::UpVector,
		FVector::DownVector
	};

	enum class Type {
		Null, Air, Stone, Dirt, Grass
	};

	const FVector Vertices[8] = {
		FVector(100, 100, 100), // Far,  Right, High
		FVector(100,   0, 100), // Far,  Left,  High
		FVector(100,   0,   0), // Far,  Left,  Low
		FVector(100, 100,   0), // Far,  Right, Low
		FVector(  0,   0, 100), // Near, Left,  High
		FVector(  0, 100, 100), // Near, Right, High
		FVector(  0, 100,   0), // Near, Right, Low
		FVector(  0,   0,   0)  // Near, Left,  Low
	};

	constexpr int Triangles[6][4] = {
		{ // Front/Away from the camera
			0, // Far, Right, High
			1, // Far, Left,  High
			2, // Far, Left,  Low
			3  // Far, Right, Low
		}, // Front
		{ // Back/Toward the camera
			4, // Near, Left,  High
			5, // Near, Right, High
			6, // Near, Right, Low
			7  // Near, Left,  Low
		}, // Back
		{ // Left
			1, // Far,  Left,  High
			4, // Near, Left,  High
			7, // Near, Left,  Low
			2  // Far,  Left,  Low
		}, // Left
		{ // Right
			5, // Near, Right, High
			0, // Far,  Right, High
			3, // Far,  Right, Low
			6  // Near, Right, Low
		}, // Right
		{ // Up
			5, // Near, Right, High
			4, // Near, Left,  High
			1, // Far,  Left,  High
			0  // Far,  Right, High
		}, // Up
		{ // Down
			3, // Far,  Right, Low
			2, // Far,  Left,  Low
			7, // Near, Left,  Low
			6  // Near, Right, Low
		} // Down
	};
}
