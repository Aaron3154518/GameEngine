import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ParameterVisualizerComponent } from './parameter-visualizer.component';

describe('ParameterVisualizerComponent', () => {
  let component: ParameterVisualizerComponent;
  let fixture: ComponentFixture<ParameterVisualizerComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ ParameterVisualizerComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(ParameterVisualizerComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
