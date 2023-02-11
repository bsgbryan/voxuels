// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

namespace Block {
	enum class Face {
		Front,
		Right,
		Back,
		Left,
		Up,
		Down
	};

	constexpr Face AllFaces[6] = {
		Face::Front,
		Face::Right,
		Face::Back,
		Face::Left,
		Face::Up,
		Face::Down
	};

	const FVector Vectors[6] = {
		FVector::ForwardVector,
		FVector::RightVector,
		FVector::BackwardVector,
		FVector::LeftVector,
		FVector::UpVector,
		FVector::DownVector
	};

	enum class Type {
		Null, Air, Stone, Dirt, Grass
	};

	const FVector Vertices[8] = {
		FVector(100, 100, 100),
		FVector(100,   0, 100),
		FVector(100,   0,   0),
		FVector(100, 100,   0),
		FVector(  0,   0, 100),
		FVector(  0, 100, 100),
		FVector(  0, 100,   0),
		FVector(  0,   0,   0)
	};

	constexpr int Triangles[6][4] = {
		{ 0, 1, 2, 3 }, // Front
		{ 5, 0, 3, 6 }, // Right
		{ 4, 5, 6, 7 }, // Back
		{ 1, 4, 7, 2 }, // Left
		{ 5, 4, 1, 0 }, // Up
		{ 3, 2, 7, 6 }  // Down
	};
}
