import { ComponentFixture, TestBed } from '@angular/core/testing';

import { TypeModalComponent } from './type-modal.component';

describe('TypeModalComponent', () => {
  let component: TypeModalComponent;
  let fixture: ComponentFixture<TypeModalComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ TypeModalComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(TypeModalComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
